<?php

/**
 * Our "man on the inside" for unit tests.
 *
 * This class adds every event to the $history atribute so that it
 * can be audited after the a run of the workflow even if the workflow
 * fails.
 *
 * In theory much of the history could be derived from the ConductorState
 * object but this gives us a a copy of the state at the time before other
 * parts of the system could possibly mess with our history.
 */
class ConductorObserverTest implements ConductorObserver {

  /**
   * Tracks a copy of every event and the object that it happened to
   * (in the state it was in when the event happened) for testing.
   */
  public $history = array();

  public function notify($event, $object) {
    if (!is_object($object)) {
      $backtrace = debug_backtrace();
      throw new Exception('Non-object passed to observers.');
    }
    $non_reference_copy = clone $object;
    // TODO: Ensure we're cloning at the appropriate depth.
    // foreach ($things_to_unset as $thing) {
    //  unset($non_reference_copy->{$thing});
    // }
    $this->history[$event][] = $object;
  }

}
