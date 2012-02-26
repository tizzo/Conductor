<?php

/**
 * This is a test activity to perform different actions depending on the context data.
 */
class ConductorActivityTest extends ConductorActivity {

  /**
   * Implements ConductorActivity::run().
   */
  public function run() {
    if ($this->state->getContext($this->name . ':pause') == TRUE && $this->state->getContext($this->name . ':resume') == FALSE) {
      $this->activityState->markSuspended();
    }
    else {
      $this->activityState->markCompeted();
    }
  }

}
