<?php
/**
 * @file workflow.inc
 *   Provides the workflow object type and associated methods.
 */

/**
 * An object to contain all of the data for a workflow, plus the member
 * functions to process the workflow.
 */
class ConductorWorkflow {

  // The base database table of this object.
  protected  $db_table = 'conductor_workflow';

  // An array of ConductorActivity descended objects comprising this workflow.
  public $activities = array();

  // The machine name of this workflow.
  public $name = '';

  // The human readable title of this workflow.
  public $title = '';

  // A ConductorState object containing state for a workflow instance.
  public $state = NULL;

  /**
   * Allow the workflow to be populated with state on load.
   */
  public function __construct($state = NULL) {
    if (!is_null($state)) {
      $this->setState($state);
    }
  }

  /**
   * Save the workflow to the database. If the workflow does not already exist,
   *
   * A wid will be assigned to the workflow and returned from this function.
   */
  public function save() {
    if (!isset($this->name)) {
      return FALSE;
    }
    $update = FALSE;
    if (!isset($this->wid) || $this->wid == 'new') {
      $this->wid = NULL;
      $update = TRUE;
    }
    if ($update){
      $query = db_insert('conductor_workflow');
    }
    else {
      $query = db_update('conductor_workflow')
        ->condition('wid', $this->wid);
    }

    $schema = drupal_get_schema($this->db_table);
    $fields = array();

    // We take responsibility for the activities so remove them from the schema.
    unset($schema['fields']['activities']);
    // Serialize the activities
    $fields['activities'] = serialize($this->activities);

    foreach ($schema['fields'] as $name => $field) {
      if (isset($this->$name)) {
        $fields[$name] = $this->$name;
      }
    }
    return $query
      ->fields($fields)
      ->execute();
  }

  /**
   * Load a workflow from the database based upon either wid or name.
   *
   * This is a static factory method that implements internal caching for
   * workflow objects.
   *
   * @param $arg
   *  The name of the workflow or its internal workflow id (wid)
   * @param $reset
   *  If TRUE, reset this entry in the load cache.
   * @return workflow
   *   A workflow object or NULL if it was not available.
   */
  static function &load($arg, $reset = FALSE) {

    static $cache = &drupal_static('__FUNCTION__',.array());

    // We want a NULL value to return TRUE here, so we can't use isset() or empty().
    if (!array_key_exists($arg, $cache) || $reset) {
      $result = db_select('conductor_workflow', 'w')
        ->fields('w')
        ->condition(is_numeric($arg) ? 'wid' : 'name', $arg)
        ->execute()
        ->fetchObject();
      if (isset($result->data)) {
        $workflow = unserialize($result->data);
      }
      else {
        return NULL;
      }
      $workflow->loaded = TRUE;
      $cache[$arg] = $workflow;
    }
    return $cache[$arg];
  }

  /**
   * Delete the workflow from the database.
   */
  public function delete() {
    db_delete('conductor_workflow')
      ->condition('wid', $this->wid)
      ->execute();
  }

  /**
   * Export a workflow as PHP code.
   */
  function export($indent = '') {

    $this->indexActivities();
    $output = '';
    $output .= "\n" . $indent . "/* Conductor Workflow: " . $this->title . " */\n";
    $output .= $this->export_row('workflow', $indent);
    // Set the API version
    $api = conductor_ctools_plugin_api('conductor', 'workflow');
    $output .= $indent . '$workflow->api_version = \'' . $api['version'] . "';\n";
    $output .= $indent . '$workflow->disabled = FALSE; /* Edit this to true to make a default workflow disabled initially */' . "\n";
    foreach ($this->activities as $id => $activity) {
      $output .= "\n" . $indent . "/* Activity: $activity->title */\n";
      $output .= $indent . '$activity = $workflow->newActivity(' . ctools_var_export($activity->plugin_info['name'], $indent) . ");\n";
      $output .= $activity->export_options($indent, '$activity');
    }
    return $output;
  }

  /**
   * Export a loaded row, such as an argument, field or the view itself to PHP code.
   * TODO: port for workflow
   *
   * @param $identifier
   *   The variable to assign the PHP code for this object to.
   * @param $indent
   *   An optional indentation for prettifying nested code.
   */
  function export_row($identifier = NULL, $indent = '') {
    ctools_include('export');

    if (!$identifier) {
      $identifier = $this->db_table;
    }
    $schema = drupal_get_schema($this->db_table);

    $output = $indent . '$' . $identifier . ' = new ' . get_class($this) . ";\n";
    // Go through our schema and build correlations.
    foreach ($schema['fields'] as $field => $info) {
      if (!empty($info['no export'])) {
        continue;
      }
      if (!isset($this->$field)) {
        if (isset($info['default'])) {
          $this->$field = $info['default'];
        }
        else {
          $this->$field = '';
        }

        // serialized defaults must be set as serialized.
        if (isset($info['serialize'])) {
          $this->$field = unserialize($this->$field);
        }
      }
      $value = $this->$field;
      if ($info['type'] == 'int') {
        if (isset($info['size']) && $info['size'] == 'tiny') {
          $value = (bool) $value;
        }
        else {
          $value = (int) $value;
        }
      }

      $output .= $indent . '$' . $identifier . '->' . $field . ' = ' . ctools_var_export($value, $indent) . ";\n";
    }
    return $output;
  }


  /**
   * Create and add an activity with a handler to the workflow.
   */
  public function newActivity($activity) {
    $new_activity = conductor_get_activity_handler($activity);
    $new_activity->workflow = &$this;
    $this->activities[] = $new_activity;
    return $new_activity;
  }

  public function getActivity($name) {
    $activity = FALSE;
    // If we can't find an activity by that name we may need to reindex activities.
    if (!isset($this->activities[$name])) {
      $this->indexActivities();
    }
    // If we have an activity of the requested name, return it.
    if (isset($this->activities[$name])) {
      $activity = $this->activities[$name];
    }
    return $activity;
  }

  public function getActivities() {
    return $this->activities;
  }

  /**
   * Set keys of the activity's name for each activity.
   */
  public function indexActivities() {
    $activities = $this->activities;
    $this->activities = array();
    foreach ($activities as &$activity) {
      $this->activities[$activity->name] = $activity;
    }
  }


  /**
   * Safely clone a workflow.
   *
   * Because workflows are complicated objects within objects, and PHP loves to
   * do references to everything, if a workflow is not properly and safely
   * cloned it will still have references to the original workflow, and can
   * actually cause the original workflow to point to objects in the cloned
   * workflow. This gets ugly fast.
   *
   * This will completely wipe a workflow clean so it can be considered fresh.
   *
   * @return workflow
   *    The cloned workflow.
   */
  function cloneWorkflow() {
    $clone = clone($this);

    // shallow cloning means that all the child objects
    // *were not cloned*. We must clone them ourselves.
    $activities = array();
    foreach ($clone->activities as $id => $activity) {
      $activities[$id] = clone $activity;
    }
    $clone->activities = $activities;
    return $clone;
  }

  /**
   * Unset references so that a $workflow object may be properly garbage
   * collected.
   */
  function destroy() {
    unset($this->state);
    foreach ($this->activities as $activity) {
      unset($activity);
    }
  }

  /**
   * Sets the state of this workflow.
   */
  function setState(ConductorState $state) {
    // Pass a copy of this workflow into the state object.
    $state->workflow = &$this;
    $this->state = &$state;
  }

  /**
   * Gets the state of this workflow.
   *
   * @return
   *   The ConductorState descended object that manages state for this workflow.
   */
  function getState() {
    return $this->state;
  }

  /**
   * Run this workflow.
   */
  function run() {
    $tries = &drupal_static('__FUNCTION__', 0);
    if ($this->getState()) {
      // Figure out what we need to do to resume.
    }
    else {
      // Figure out what more we need to do to start anew.
      $this->setState(new ConductorState);
    }
    // Get an ID for this instance.
    $this->instanceID = $this->state->getIntanceID();
    // Acquire a lock for this instance, ensuring 2 processes do not run this
    // workflow simultaneously.
    if (lock_acquire($this->instanceID)) {
      // Ensure that activities are indexed.
      $this->indexActivities();
      // TODO: Create a record for this instance in the appropriate place.
      // Get the active activities from the state.

      // Loop through all active Activities processing them until we
      // stop getting a current set of activeActivities that are different
      // from the ones we already have (indicating that all active activities
      // can't yet run and are blocked on some external process).
      $activeActivities = array();
      $newActiveActivities = $this->state->getActiveActivities();
      if (is_array($newActiveActivities)) {
        // Verify that we still have activities to run and all are not stalled.
        while (count(array_diff(array_keys($newActiveActivities), array_keys($activeActivities)))) {
          $activeActivities = $newActiveActivities;
          foreach ($activeActivities as $name => &$activityState) {
            $activity = $this->activities[$name];
            $activity->setState($activityState);
            if ($activity->checkRunnability()) {
              $activityState = $activity->run();
              // TODO: deal with other states.
              if ($activityState->status() === ConductorActivityState::FINISHED) {
                $this->state->completeActivity($activity, $activityState);
              }
            }
          }
          // Retrieve any newly eligible activities to process.
          $newActiveActivities = $this->state->getActiveActivities();
        }
        return TRUE;
      }
    }
    else if ($tries < 5) {
      $tries++;
      $this->run();
    }
    else {
      return FALSE;
    }
  }

  function runActiveActivities() {
  }

}
