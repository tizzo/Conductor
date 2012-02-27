<?php

/**
 * This is a test activity to perform different actions depending on the context data.
 */
class ConductorActivityTest extends ConductorActivity {

  /**
   * Implements ConductorActivity::run().
   */
  public function run() {
    if ($this->getState()->getContext($this->name . ':suspend') == TRUE && $this->getState()->getContext($this->name . ':resume') == FALSE) {
      $this->activityState->markSuspended();
    }
    else {
      $this->activityState->markCompeted();
    }
  }

  /**
   * Implements ConductorActivity::getSuspendPointers().
   */
  public function getSuspendPointers() {
    return array(
      $this->name . ':suspended',
    );
  }

}
