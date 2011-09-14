<?php

/**
 *
 */
class ConductorActivity extends ConductorObject {

  //
  public $inputs = array();

  //
  public $outputs = array();

  //
  public $x = null;

  // 
  public $y = null;

  public function option_definition() {
    $options['name'] = array('default' => '');
    $options['title'] = array('default' => '');
    $options['x'] = array('default' => 0);
    $options['y'] = array('default' => 0);
    $options['inputs'] = array('default' => array());
    $options['outputs'] = array('default' => array());
    return $options;
  }

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
   * @defgroup conductor_activity_ui Conductor Activity UI
   * @{
   * UI Related elements of the Conductor Activity API.
   */

  /**
   *
   */
  public function getUILinks() {
    // Create an array to be rendered by theme_links__ctools_dropbutton().
    $ctools_modal_attributes = array(
      'class' => array(
        'use-ajax',
        'ctools-use-modal'
      ),
    );
    $links = array();
    $links['edit'] = array('title' => t('edit'), 'href' => 'conductor_ui/activity-config/' . strtr($this->workflow->name, array('_' => '-')) . '/' . strtr($this->name, array('_' => '-')) . '/no-js', 'attributes' => $ctools_modal_attributes);
    $links['input'] = array('title' => t('add input'), 'href' => $_GET['q']);
    $links['output'] = array('title' => t('add output'), 'href' => $_GET['q']);
    $links['remove'] = array('title' => t('remove'), 'href' => $_GET['q']);
    foreach ($links as &$link) {
      $link['attributes']['class'][] = 'conductor-ui-activity-link';
    }
    return $links;
  }

/**
 * @} End of "Conductor Activity UI".
 */

}
