<?php

/**
 * Tracks the state of an individual activity.
 */
class ConductorActivityState {

  /**
   * The activity has been initialized but not yet processed.
   */
  const INITIALIZED = -1;

  /**
   * The activity processing has failed in a way that cannot be recovered.
   */
  const FAILED = 1;

  /**
   * The activity is paused and will need to be resumed.
   */
  const PAUSED = 2;

  /**
   * The activity ran, but must be ran again.
   */
  const RUN_AGAIN = 3;

  /**
   * The activity is finished and should not be run again.
   */
  const FINISHED = 0;

  // The status of this activity, one of the constants listed above.
  protected $status = NULL;

  // The plugin of this activity.
  public $plugin = '';

  // The name of the activity.
  public $name = '';

  // An array of ConductorActivityState descended objects that represent
  // the state of the inputs for this activity.
  public $inputStates = array();

  // A container for the 
  public $workflowState = NULL;

  // An optional context to be passed on the the next item.
  public $context = array();

  public function __construct(ConductorActivity $activity, ConductorState $state) {
    $this->name = $activity->name;
    $this->plugin = $activity->plugin_info['name'];
    $this->status = self::INITIALIZED;
    $this->workflowState = &$state;
  }

  /**
   * The status of this activity.
   */
  public function status() {
    return $this->status;
  }

  public function markFinished() {
    $this->status = self::FINISHED;
  }

  public function markRunAgain() {
    $this->status = self::RUN_AGAIN;
  }

  public function setInputActivityState(ConductorActivityState $state) {
    $this->inputStates[$state->name] = $state;
    if (is_array($state->context) && is_array($this->context)) {
      $this->context = array_merge($state->context, $this->context);
    }
  }

  /**
   * Retrieve the Activity State of one of the inputs.
   */
  public function getInputActivityState($name = NULL) {
    $state = FALSE;
    if (!is_null($name) && isset($this->inputStates[$name])) {
      $state = $this->inputStates[$name];
    }
    return $state;
  }
}
