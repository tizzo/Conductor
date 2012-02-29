<?php

/**
 * Provides the default handler for Conductor Storage using the dabase.
 *
 * TODO: Should we 
 */
class ConductorStorageDatabase implements ConductorStorage {

  protected $options = array();

  /**
   * Set the options to be used in these database connections.
   *
   * @param $options
   *   An options array as would be passed to db_select, db_update, etc.
   */
  public function setOptions(array $options) {
    $this->options = $options;
  }

  public function save(stdClass $data) {
    $record = new stdClass;
    // TODO: store workflow name
    $record->data = $data;
    $keys = array();
    if (isset($data->uniqueId) && is_numeric($data->uniqueId)) {
      $keys[] = 'id';
      $record->id = $data->uniqueId;
      unset($record->data->uniqueId);
    }
    drupal_write_record('conductor_instance', $record, $keys);
    return $record->id;
  }

  public function load($uniqueId) {
    $result = db_query('SELECT * FROM {conductor_instance} WHERE id=:id', array('id' => $uniqueId), $this->options)->fetchObject();
    if ($result) {
      $return = unserialize($result->data);
      $return->uniqueId = $result->id;
      return $return;
    }
    else {
      return FALSE;
    }
  }

  public function loadPointer($pointerKey) {
    $pointer = db_query('SELECT * FROM {conductor_instance_pointer} WHERE pointer_key = :pointer_key', array('pointer_key' => $pointerKey), $this->options)
      ->fetchObject();
    if (is_object($pointer)) {
      return array(
        'workflowName' => $pointer->workflow_name,
        'instanceId' => $pointer->sid,
        'activityName' => $pointer->activity_name,
      );
    }
    else {
      return FALSE;
    }
  }

  public function delete($uniqueId) {
    db_delete('conductor_instance')
     ->condition('id', $uniqueId)
     ->execute();
  }

  public function savePointer($workflowName, $instanceId, $activityName, $pointerKey) {
    $record = new stdClass;
    $record->workflow_name = $workflowName;
    $record->sid = $instanceId;
    $record->activity_name = $activityName;
    $record->pointer_key = $pointerKey;
    drupal_write_record('conductor_instance_pointer', $record);
  }

  public function deletePointer($pointerKey) {
    db_delete('conductor_instance_pointer')
     ->condition('pointer_key', $pointerKey)
     ->execute();
  }

}

