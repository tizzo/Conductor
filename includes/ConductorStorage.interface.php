<?php

interface ConductorStorage {

  /**
   * Implements ConductorStorage::save().
   *
   * @param $state
   *   The ConductorInstance state object to be saved.
   */
  public function save(stdClass $data);

  /**
   * Implements ConductorStorage::load().
   *
   * @return
   *   A loaded ConductorInstance object.
   */
  public function load($uniqueId);

  /**
   * Implements ConductorStorage::loadFromPointer().
   *
   * @param $name
   *   The name of the pointer.
   */
  public function loadPointer($pointerKey);

  /**
   * Implements ConductorStorage::delete().
   */
  public function delete($unique_id);

  /**
   * Save pointers associated with an activity and this workflow.
   */
  public function savePointer($workflowName, $instanceId, $activityName, $pointerKey);

  /**
   * Implements ConductorStorage::deletePointer().
   */
  public function deletePointer($pointerKey);

}
