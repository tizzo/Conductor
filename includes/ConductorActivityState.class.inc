<?php

/**
 * Tracks the state of an individual activity.
 */
class ConductorActivityState {

  const INITIALIZED = -1;

  const FAILED = 1;

  const PAUSED = 2;

  const RUN_AGAIN = 3;

  const FINSIHED = 0;

  // The status of this activity, one of the constants listed above.
  protected $status = NULL;

  // The plugin of this activity.
  public $plugin = '';

  // The name of the activity.
  public $name = '';

  public function __construct(ConductorActivity $activity) {
    $this->name = $activity->name;
    $this->plugin = $activity->plugin_info['name'];
    $this->status = self::INITIALIZED;
  }

  /**
   * The status of this activity.
   */
  function status() {
    return $status;
  }
}
