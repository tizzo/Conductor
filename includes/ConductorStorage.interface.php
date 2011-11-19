<?php

interface ConductorStorage {

  /**
   *
   * @param $state
   *   The ConductorInstance state object to be saved.
   */
  public function save(ConductorInstance $state);

  /**
   *
   * @return
   *   A loaded ConductorInstance object.
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
