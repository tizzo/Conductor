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

  /**
   *
   */
  public function getUILinks() {
    // Create an array to be rendered by theme_links__ctools_dropbutton().
    $links = array();
    $links['edit'] = array('title' => t('edit'), 'href' => $_GET['q']);
    $links['input'] = array('title' => t('add input'), 'href' => $_GET['q']);
    $links['output'] = array('title' => t('add output'), 'href' => $_GET['q']);
    return $links;
  }
}
