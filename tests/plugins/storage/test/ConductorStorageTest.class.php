<?php

/**
 *
 */
class ConductorStorageTest implements ConductorStorage {

  /**
   * A multidimensional array of values 
   */
  private $storage = array();

  /**
   *
   */
  public function setDatabase(array &$storage) {
    $this->storage = &$storage;
  }

  /**
   * Implements ConductorStorage::save().
   *
   * @param $state
   *   The ConductorInstance state object to be saved.
   */
  public function save(stdClass $data) {
    $instances = &$this->storage['instances'];
    if (!isset($this->storage['instances'])) {
      $this->storage['instances'] = array();
    }
    // Note we serialize our data here to more closely simulate actual storage.
    $data->uniqueId = count($this->storage['instances']) + 1;
    return (array_push($this->storage['instances'], serialize($data)) - 1);
  }

  /**
   * Implements ConductorStorage::load().
   *
   * @return
   *   A loaded ConductorInstance object.
   */
  public function load($uniqueId) {
    if (isset($this->storage['instances'][$uniqueId])) {
      return unserialize($this->storage['instances'][$uniqueId]);
    }
  }

  /**
   * Implements ConductorStorage::loadFromPointer().
   *
   * @param $name
   *   The name of the pointer.
   */
  public function loadPointer($pointerKey) {
    if (isset($this->storage['pointers'][$pointerKey])) {
      return $this->storage['pointers'][$pointerKey];
    }
    else {
      return FALSE;
    }
  }

  /**
   * Implements ConductorStorage::delete().
   */
  public function delete($unique_id) {
    unset($this->storage['instances'][$unique_id]);
  }

  /**
   * Save pointers associated with an activity and this workflow.
   */
  public function savePointer($workflowName, $instanceId, $activityName, $pointerKey) {
    if (!isset($this->storage['pointers'])) {
      $this->storage['pointers'][$pointerKey] = array(
        'workflowName' => $workflowName,
        'instanceId' => $instanceId,
        'activityName' => $activityName,
      );
    }
  }

  /**
   * Implements ConductorStorage::deletePointer().
   */
  public function deletePointer($pointerKey) {
    unset($this->storage['pointers'][$pointerKey]);
  }
}
