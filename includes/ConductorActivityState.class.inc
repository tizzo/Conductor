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

  // An optional context to be passed on the the next item.
  public $context = array();

  public function __construct(ConductorActivity $activity) {
    $this->name = $activity->name;
    $this->plugin = $activity->plugin_info['name'];
    $this->status = self::INITIALIZED;
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

  public function markSuspended() {
    $this->status = self::SUSPENDED;
  }

  public function markFailed() {
    $this->status = self::FAILED;
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

  /**
   * Set context for this activity.
   *
   * If one parameter is given and it is an array, reset the context to that array.
   * if two parameters are given, set them as a key => value pair to the existing
   * context array.
   *
   * @param $one
   *   If two is present the key of the context value to set, if two is not
   *   present then an array to set this context to.
   * @param $two
   *   The value of the context value to set.
   */
  public function setContext($one, $two = NULL) {
    $args = func_get_args();
    if (count($args) == 2) {
      $this->context[$one] = $two;
    }
    else if (count($args) == 1 && is_array($one)) {
      $this->context = $one;
    }
    else {
      throw new Exception('Invalid context provided to workflow instance in method `setContext`.');
    }
  }

  /**
   * Get the context for this activity.
   */
  public function getContext($name = FALSE) {
    if (!$name) {
      $return = $this->context;
    }
    else {
      if (isset($this->context[$name])) {
        $return = $this->context[$name];
      }
      else {
        $return  = FALSE;
      }
    }
    return $return;
  }
}
