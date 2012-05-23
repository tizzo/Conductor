<?php

/**
 *
 */
class ConductorActivity extends ConductorObject {

  /**
   * An array of other activity names that feed into this activity.
   *
   * TODO: We may need to expand this to provide information about allowable contexts.
   */
  public $inputs = array();

  /**
   * An array of other activity names that feed out of this activity.
   *
   * TODO: We may need to expand this to provide information about provided contexts.
   */
  public $outputs = array();

  // The number of pixels from the top of the container this activity should appear.
  public $x = null;

  // The number of pixels from the left of the container this activity should appear.
  public $y = null;

  // The name of the plugin used for this handler.
  public $plugin = '';

  // The ConductorActivityState object tracking the state of this activity.
  public $activityState = FALSE;

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
   * Set the activity state on this activity.
   *
   * @param $activityState
   *   The state of this individual activity.
   */
  public function setState($activityState) {
    $this->activityState = $activityState;
  }

  /**
   * Get the activity state for this object (creating one if it does not exist).
   *
   * @return ConductorActivityState
   *   A ConductorActivityState object.
   */
  public function &getState() {
    if (!$this->activityState) {
      $this->activityState = new ConductorActivityState($this);
    }
    return $this->activityState;
  }

  public function checkRunnability() {
    $runnable = TRUE;
    if (is_null($this->activityState)) {
      $runnable = FALSE;
    }
    foreach ($this->inputs as $input) {
      // TODO: Currently the base Condcutor Activity defaults to requiring
      // that all configured inputs have been properly run before allowing
      // itself to run.  Decide if there should be configuration available
      // by default or if we should have a different default.

      // Ensure we have state for every configured input.
      if ($this->getState()->getInputActivityState($input)) {
        $inputState = $this->getState()->getInputActivityState($input);
        if ($inputState->getStatus() != ConductorActivityState::COMPLETED) {
          $runnable = FALSE;
        }
      }
    }
    return $runnable;
  }

  /**
   * Run this activity performing whatever actions need to be performed.
   *
   * @return
   *   The appropriately adjusted ConductorActivityState object.
   */
  public function run() {
    if ($this->process()) {
      $this->activityState->markCompeted();
    }
    else {
      $this->activityState->markFailed();
    }
  }

  /**
   * Process this workflow item.
   *
   * TODO: We may want to allow the process method to have more smarts
   * so the run method can hide its inner workings to consumers of the
   * API and still offer the same functionality (pause, etc.).
   *
   * @return
   *   TRUE if the process completed successfully.
   */
  public function process() {
    return TRUE;
  }

  /**
   * Generate a unique pointer that identifies this activity in this workflow.
   *
   * When workflows are suspended it is usually because some step in
   * the workflow requires external input.  The storage interface was
   * designed to work with easily shardable key-value stores relying
   * on serialized PHP to flexibly persist arbitrary state information
   * in a unified way.  The pointer generated here will need to be
   * predictable to external code (prefered) or, itself, be persisted
   * somewhere such that this instance can be found.
   *
   * @param $workflow
   *   A lot of context may be required to properly create the unique ID.
   *   Handing in the workflow is the best way to make sure everything is
   *   available.
   * @return
   *   Mixed.  A linear array of pointers or FALSE if there are none.
   */
  public function getSuspendPointers(ConductorWorkflow $workflow) {
    return FALSE;
  }

  /**
   * TODO:
   */
  public function configureForm(&$form) {
    // TODO: Once testing is competed, this should be the default form.
    // TODO: Add an option here to specify whether we should wait for all inputs to be completed or allow for any one to move processing forward.
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

  /**
   * @defgroup conductor_activity_ui Conductor Activity UI
   * @{
   * UI Related elements of the Conductor Activity API.
   */

  /**
   * TODO: How can we delegate this to the conductor_ui module?
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
