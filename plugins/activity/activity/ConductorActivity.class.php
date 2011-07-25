<?php

/**
 *
 */
class ConductorActivity {

  //
  public $inputs = array();

  //
  public $outputs = array();

  //
  public $x = null;

  // 
  public $y = null;

  public function addInput($activity) {
  }

  public function addOutput($activity) {
  }

  /**
   * TODO:
   */
  public function configureForm() {
    return FALSE;
  }

  /**
   * TODO:
   */
  public function configureFormValidate() {
  }

  /**
   * TODO:
   */
  public function configureFormSubmit() {
  }

  public function run() {
  }

  public function getId() {
    // TODO: Consider whether this could be made any smarter...
    return drupal_hash_base64($this) . REQUEST_TIME;
  }
}
