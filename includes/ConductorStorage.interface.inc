<?php

interface ConductorStateStorage {

  /**
   *
   * @param $state
   *   The ConductorState state object to be saved.
   */
  public function save(ConductorState $state);

  /**
   *
   * @return
   *   A loaded ConductorState object.
   */
  static function load($unique_id);

  /**
   * @param $name
   *   The name of the pointer.
   */
  static function loadFromPointer($name);

  /**
   *
   */
  static function delete($unique_id);

  /**
   *
   */
  static function deletePointer($name);
}
