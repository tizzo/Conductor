<?php

/**
 * This is the first activity in any workflow.
 */
class ConductorActivityPauseTest extends ConductorActivity {

  /**
   * Implements ConductorActivity::run().
   */
  public function run() {
    //if ($this->state->getContext('pauseTest') === TRUE) {
      $this->activityState->markSuspended();
    //}
  }

  /**
   * The start method performs no actions.
   */
  public function process() {
    // TODO: Start returning other the pause status.
    return TRUE;
  }

}
