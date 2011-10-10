<?php

/**
 * Provides a simple observer interface.
 *
 * All loggers are descended from this class.
 */
interface ConductorObserver {

  /**
   * Receive a notification.
   */
  public function notify($name, $object);

}
