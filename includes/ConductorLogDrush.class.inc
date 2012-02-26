<?php

/**
 * Provides an easy way to see into ConductorWorkflows as
 * they are run from drush commands.
 */
class ConductorLogDrush extends ConductorLog {

  /**
   * Implements ConductorLog::logEvent().
   */
  public function logEvent($event, $object, $logLevel) {

    // Determine the drush_log "type" to use.
    if ($logLevel < 0) {
      $level = 'notice';
    }
    else if ($logLevel == 0) {
      $level = 'ok';
    }
    else {
      $level = 'error';
    }

    // If the log threshold is less than what is specified, do not log.
    if ($level >= $this->logThreshold) {
      list($string, $args) = $this->generateHumanMessage($event, $object);
      drush_log(t($string, $args), $level);
    }
  }

}
