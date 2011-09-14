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
  public function configureForm(&$form) {
    // TODO: Once testing is finished, this should be the default.
    //return FALSE;
    $form['human_name'] = array(
      '#type' => 'textfield',
      '#title' => t('workflow name'),
      '#required' => TRUE,
      '#size' => 32,
      '#default_value' => !empty($form_state['workflow']) ? $form_state['workflow']->human_name : '',
      '#maxlength' => 255,
    );
    $form['name'] = array(
      '#type' => 'machine_name',
      '#maxlength' => 32,
      '#machine_name' => array(
        'exists' => 'conductor_get_workflow',
        'source' => array('human_name'),
      ),
      '#description' => t('A unique machine-readable name for this View. It must only contain lowercase letters, numbers, and underscores.'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Save',
    );
    return $form;
  }

  /**
   * TODO:
   */
  public function configureFormValidate($form, &$form_state) {

  }

  /**
   * TODO:
   */
  public function configureFormSubmit($form, &$form_state) {
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
    $links['edit'] = array('title' => t('edit'), 'href' => 'conductor_ui/activity-config/' . strtr($this->workflow->name, array('_' => '-')) . '/' . strtr($this->name, array('_' => '-')) . '/nojs', 'attributes' => $ctools_modal_attributes);
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
