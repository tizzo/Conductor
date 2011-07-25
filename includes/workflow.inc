<?php
/**
 * @file workflow.inc
 * Provides the workflow object type and associated methods.
 */

/**
 * An object to contain all of the data for a workflow, plus the member
 * functions to process the workflow.
 */
class ConductorWorkflow {

  protected  $db_table = 'conductor_workflow';
  
  public $activities = array();

  /**
   * Save the workflow to the database. If the workflow does not already exist,
   * A wid will be assigned to the workflow and also returned from this function.
   */
  public function save() {
    if ($this->wid == 'new') {
      $this->wid = NULL;
    }

    // If we have no wid or our wid is a string, this is a new workflow.
    if (!empty($this->wid)) {
      // remove existing table entries
      foreach ($this->dbObjects() as $key) {
        db_delete('conductor_' . $key)
          ->condition('wid', $this->wid)
          ->execute();
      }
    }

    $this->save_row(!empty($this->wid) ? 'wid' : FALSE);

    // Save all of our subtables.
    foreach ($this->dbObjects() as $key) {
      $this->_save_rows($key);
    }

    $this->save_locale_strings();
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
    static $cache = array();

    // We want a NULL value to return TRUE here, so we can't use isset() or empty().
    if (!array_key_exists($arg, $cache) || $reset) {
      $result = db_select('conductor_workflow', 'w')
        ->fields('w')
        ->condition(is_numeric($arg) ? 'wid' : 'name', $arg)
        ->execute();
      if ($result->rowCount() == 0) {
        $cache[$arg] = NULL;
      }
      else {
        $workflow = new ConductorWorkflow();
        $workflow->loadRow($result->fetchObject());
        // Load all of our subtables.
        foreach ($workflow->dbObjects() as $key) {
          $object_name = "conductor_$key";
          $result = db_select($object_name, 'w')
            ->fields('w')
            ->condition('wid', $workflow->wid)
            ->orderBy('position')
            ->execute();
          foreach ($result as $data) {
            $object = new $object_name(FALSE);
            $object->loadRow($data);

            // Because it can get complicated with this much indirection,
            // make a shortcut reference.
            $location = &$workflow->$key;

            // If we have a basic id field, load the item onto the workflow based on
            // this ID, otherwise push it on.
            if (!empty($object->id)) {
              $location[$object->id] = $object;
            }
            else {
              $location[] = $object;
            }
          }
        }
        $workflow->loaded = TRUE;
        $cache[$arg] = $workflow;
      }
    }

    return $cache[$arg];
  }

  /**
   * Load the object with a row from the database.
   *
   * This method is separate from the constructor in order to give us
   * more flexibility in terms of how the workflow object is built in different
   * contexts.
   *
   * @param $data
   *   An object from db_fetch_object. It should contain all of the fields
   *   that are in the schema.
   */
  public function loadRow($data) {
    $schema = drupal_get_schema($this->db_table);

    // Go through our schema and build correlations.
    foreach ($schema['fields'] as $field => $info) {
      $this->$field = empty($info['serialize']) ? $data->$field : unserialize($data->$field);
    }
  }

  /**
   * Returns the complete list of dependent objects in a workflow, for the purpose
   * of initialization and loading/saving to/from the database.
   */
  static function dbObjects() {
    return array();
  }

  /**
   * Create and add an activity with a handler to the workflow.
   */
  function newActivity($activity) {
    $new_activity = conductor_get_activity_handler($activity);
    $this->activities[] = $new_activity;
    return $new_activity;
  }

}
