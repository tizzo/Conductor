<?php

/**
 * This is a test activity to perform different actions depending on the context data.
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
