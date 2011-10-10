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
   * The activity is being processed.
   */
  const ACTIVE = 0;

  /**
   * The activity processing has failed in a way that cannot be recovered.
   */
  const FAILED = 1;

  /**
   * The activity must be re-run at another time,
   * but definitively not during this run.
   */
  const SUSPENDED = 2;

  /**
   * The activity ran, but must run again.
   */
  const RUN_AGAIN = 3;

  /**
   * The activity is completed and should not be run again.
   */
  const COMPLETED = 4;


  // The status of this activity, one of the constants listed above.
  protected $status = NULL;

  // The plugin of this activity.
  public $plugin = '';

  // The name of the activity.
  public $name = '';

  // An array of ConductorActivityState descended objects that represent
  // the state of the inputs for this activity.
  public $inputStates = array();

  // A container for the state for this entire workflow instance.
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
  public function getStatus() {
    return $this->status;
  }

  /**
   * @param $status
   *   Status must contain one of the status constants in this file.
   */
  public function setStatus($status) {
    $this->status = $status;
  }

// TODO: Do I really want a ton of different methods just to manipulate a single attribute?
  public function markCompeted() {
    $this->status = self::COMPLETED;
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
