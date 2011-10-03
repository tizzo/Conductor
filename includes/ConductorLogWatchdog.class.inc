<?php

class ConductorLogWatchdog extends ConductorLog {

  /**
   * Implements ConductorLog::logEvent().
   */
  public function logEvent($event, $object, $logLevel) {
    // Determine the watchdog "severty" to use.
    // Acceptable values: WATCHDOG_EMERGENCY, WATCHDOG_ALERT, WATCHDOG_CRITICAL,
    // WATCHDOG_ERROR, WATCHDOG_WARNING, WATCHDOG_NOTICE, WATCHDOG_INFO,
    // WATCHDOG_DEBUG

    if ($logLevel < 0) {
      $severity = WATCHDOG_DEBUG;
    }
    else if ($logLevel == 0) {
      $level = WATCHDOG_NOTICE;
    }
    else {
      $level = WATCHDOG_ERROR;
    }

    // TODO: We should allow our messages to be translated properly.
    // This means a change in generateHumanMessage.
    list($string, $args) = $this->generateHumanMessage();
    watchdog('Conductor', $string, $args, $severity);
  }

}
