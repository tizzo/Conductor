<?php
/**
 * @file workflow.inc
 *   Provides the workflow object type and associated methods.
 */

/**
 * An object to contain all of the data for a workflow, plus the member
 * functions to process the workflow.
 *
 * TODO: Rename getState to getInstance.
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

  // A ConductorInstance object containing state for a workflow instance.
  public $state = FALSE;

  /**
   * Allow the workflow to be populated with state on load.
   */
  public function __construct($state = FALSE) {
    if ($state) {
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

    $cache = &drupal_static('__FUNCTION__', array());

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
   * Create and add an activity with a handler to this workflow.
   *
   * @param $activityHandlerName
   *   The name of the ctools plugin for this activity handler.
   * @return
   *   If successfully created, the new ConductorActivity descended object, else FALSE.
   */
  public function newActivity($activityHandlerName, $handler = FALSE) {
    if (!$activity = $handler) {
      $activity = conductor_get_activity_handler($activityHandlerName);
    }
    $activity->workflow = &$this;
    // Add the activity with an empty name because the name has probably not been set yet.
    // The name key will get into the activities array with ConductorWorkflow::indexActivities() is called.
    $this->activities[] = $activity;
    return $activity;
  }

  /**
   * Get an activity that is already a member of this workflow by machine name.
   *
   * @param $activityName
   *   The machine name of the activity in this workflow.
   * @return
   *   If found, the ConductorActivity descended activity, else FALSE.
   */
  public function &getActivity($activityName) {
    $activity = FALSE;
    // If we can't find an activity by that name we may need to reindex activities.
    if (!isset($this->activities[$activityName])) {
      $this->indexActivities();
    }
    // If we have an activity of the requested name, return it.
    if (isset($this->activities[$activityName])) {
      $activity = $this->activities[$activityName];
    }
    return $activity;
  }

  /**
   * Retrieve the first activity in the workflow.
   *
   * Currently the first activity always has the machine name "start".
   */
  public function getFirstActivity() {
    return $this->getActivity('start');
  }

  /**
   * Retrieve the last activity in the workflow.
   */
  public function getLastActivity() {
    return $this->getActivity('end');
  }

  /**
   * Get the array of all activities in this workflow.
   *
   * @return
   *   An array of all activities that are members of this workflow.
   */
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
   *
   */
  function __clone() {
    $this->state = FALSE;
    foreach ($this->activities as $id => $activity) {
      $this->activities[$id]->activityState = FALSE;
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
      // Ensure that the activityState has been cleared for each activity.
      unset($activities[$id]->activityState);
    }
    $clone->activities = $activities;
    // Ensure there is no workflow state floating around if this instance had been run.
    unset($clone->state);
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
   * Set the state of this workflow.
   *
   * Note, this can also be set using the constructor.  We currently support
   * both constructor and setter dependency injection techniques, though this is
   * subject to change.
   *
   * @param $state
   *   A ConductorInstance descended object to use for this workflow.
   */
  function setState(ConductorInstance $state) {
    // Pass a copy of this workflow into the state object.
    $state->workflow = &$this;
    $this->state = &$state;
  }

  /**
   * Get the state of this workflow.
   *
   * If no state has been instantiated, the default state class will
   * be instantiated and used.
   *
   * @return
   *   The ConductorInstance descended object that manages state for this workflow.
   */
  function getState() {
    if (!$this->state) {
      // TODO: State pretty much requires a persistence capability.
      // TODO: How do I let others set a different state?
      $this->setState(new ConductorInstance());
    }
    return $this->state;
  }

  function run() {
    // Ensure the activities are properly indexed.
    $this->indexActivities();
    $state = &$this->getState();
    $activeActivities = $state->getActiveActivities();
    while (count($activeActivities)) {
      foreach ($activeActivities as $name => &$activityState) {
        $activity = $this->activities[$name];
        // Set the activityState for this activity.
        $activity->setState($activityState);
        if ($activity->checkRunnability()) {
          try {
            $activity->run($this);
          }
          catch (Exception $e) {
            $activityState->setContext('failException', $e);
            $state->failActivity($activity);
            $state->notifyObservers('activityFailed', $activity);
          }
        }
        else {
          $state->deactivateActivity($activity);
        }
        // For easy access to the workflow's context as a whole, merge in a global context.
        $state->setContext(array_merge($state->getContext(), $activityState->getContext()));
        switch ($activityState->getStatus()) {
          // If the activity has been completed, mark it as such.
          case ConductorActivityState::COMPLETED:
            $state->notifyObservers('activityComplete', $activity);
            $state->completeActivity($activity);
            break;
          case ConductorActivityState::SUSPENDED:
            $state->suspendActivity($activity);
            break;
          case ConductorActivityState::FAILED:
            $state->failActivity($activity);
            $state->notifyObservers('activityFail', $activity);
            break;
        }
      }
      $activeActivities = $state->getActiveActivities();
    }
    // If even one activity failed...
    if (count($state->getFailedActivities())) {
      $state->workflowFailed();
      $state->notifyObservers('workflowFailed', $this);
      $state->suspend();
    }
    else if (count($state->getSuspendedActivities())) {
      $state->workflowSuspended();
      $state->suspend();
      $state->notifyObservers('workflowSuspended', $this);
    }
    else if (!count($state->getActiveActivities()) && !count($state->getSuspendedActivities())) {
      $state->workflowComplete();
      $state->notifyObservers('workflowCompleted', $this);
    }
  }

}
