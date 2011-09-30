<?php

class ConductorLogWatchdog extends ConductorLog {

  /**
   * Implements ConductorLog::logEvent().
   */
  public function logEvent($event, $object) {
    // TODO: Map severity level retrieved from deriveLogLevel() to a watchdog severity.
    // TODO: We should allow our messages to be translated properly.
    // This means a change in generateHumanMessage.
    list($string, $args) = $this->generateHumanMessage();
    watchdog('Conductor', $string, $args, $severity = WATCHDOG_NOTICE);
  }

}
