<?php

/**
 *
 */
class ConductorEnd extends ConductorActivity {

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
    unset($options['outputs']);
    return $options;
  }

  public function addInput($activity) {
  }

  public function addOutput($activity) {
    return FALSE;
  }

  public function run() {
  }

  /**
   *
   */
  public function getUILinks() {
    // Create an array to be rendered by theme_links__ctools_dropbutton().
    $links = parent::getUILinks();
    unset($links['output']);
    unset($links['remove']);
    return $links;
  }

}
