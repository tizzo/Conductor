<?php

/**
 *
 */
class ConductorStorageTest implements ConductorStorage {

  /**
   * A multidimensional array of values 
   */
  private $storage;

  /**
   *
   */
  public function setDatabase(array &$storage) {
    $this->$storage = $storage;
  }

  /**
   * Implements ConductorStorage::save().
   *
   * @param $state
   *   The ConductorInstance state object to be saved.
   */
  public function save(ConductorInstance $instance) {
    $this->storage[] = array(
    );
  }

  /**
   * Implements ConductorStorage::load().
   *
   * @return
   *   A loaded ConductorInstance object.
   */
  static function load($unique_id) {
  }

  /**
   * Implements ConductorStorage::loadFromPointer().
   *
   * @param $name
   *   The name of the pointer.
   */
  static function loadFromPointer($name) {
  }

  /**
   * Implements ConductorStorage::delete().
   */
  static function delete($unique_id) {
  }

  /**
   * Implements ConductorStorage::deletePointer().
   */
  static function deletePointer($name) {
  }
}
