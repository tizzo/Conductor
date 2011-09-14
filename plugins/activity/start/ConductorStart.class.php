<?php

/**
 *
 */
class ConductorStart extends ConductorActivity {

  //
  public $inputs = array();

  //
  public $outputs = array();

  //
  public $x = null;

  // 
  public $y = null;

  public function option_definition() {
    $options = parent::option_definition();
    unset($options['inputs']);
    return $options;
  }

  public function addInput($activity) {
    return FALSE;
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

  /**
   *
   */
  public function getUILinks() {
    // Create an array to be rendered by theme_links__ctools_dropbutton().
    $links = parent::getUILinks();
    unset($links['input']);
    unset($links['remove']);
    return $links;
  }

}
