<?php

/**
 * This class maintains the state of a Conductor workflow.
 *
 * ConductorInstance is handed into a ConductorWorkflow to track the state of a specific
 * instance of that workflow.  It delegates saving to a member object stored in the
 * $storageHandler attribute, which must implement the ConductorStorage interface.
 *
 * ConductorInstance is comprised of ConductorActivityState descended objects.
 * Each of these maintains the state of an individual activity.
 */
class ConductorInstance {

  /**
   * The workflow has been initialized with state but not yet processed.
   */
  const INITIALIZED = 0;

  /**
   * The workflow instance is in the process of being run.
   */
  const RUNNING = 1;

  /**
   * The workflow has finished running.
   */
  const FINISHED = 2;

  /**
   * The workflow failed during the run.
   */
  const FAILED = 3;

  /**
   * The workflow has been suspended to be resumed later.
   */
  const SUSPENDED = 4;

  /**
   * An array of CondcutorActivityState descended object.
   *
   * Each of these objects tracks the state for a specific activity in the
   * workflow.
   */
  protected $activityStates = array();

  /**
   * Bins are groupings of activities.
   *
   * This is used internally to track what activities are in different
   * states including active, completed, suspended, etc.  It is used to
   * ensure that we can easily serialize state without having references
   * that become duplicate objects upon unserialization.
   *
   * This multidimensional array is keyed by the bin name corresponding
   * to the state (e.g. active) and contains a linear array of activities.
   */
  protected $activityBins = array();

  /**
   * A reference to the parent workflow.
   */
  public $workflow = NULL;

  /**
   * An array of ConductorObserver objects registered to this instance.
   */
  protected $observers = array();

  /**
   * An object implementing the ConductorStorage interface.
   *
   * All state CRUD operations are delgated to this object.
   */
  public $storageHandler = NULL;

  /**
   * The status of this workflow instance.
   *
   * This should always contain one of the constants in this class.
   */
  protected $status = self::INITIALIZED;

  /**
   * If this instance has been persisted, the unique id of the instance.
   */
  protected $uniqueId = FALSE;

  /**
   * The aggregated context derived by array merging the context object
   * of each activity run so far.
   */
  protected $context = array();


  /**
   * Constructor for ConductorInstance.
   *
   * TODO: Right now this constructor can't be used practically but it
   * should make resuming working processing dead simple when loading
   * from a persistence layer.
   *
   * @param $activityStates (optional)
   *   An associative array of ConductorActivityState objects keyed by $activity->name.
   */
  public function __construct(array $activityStates = array()) {
    $this->setActivityStates($activityStates);
  }

  /**
   * Get the status of this workflow instance.
   *
   * @return
   *   An integer mapping to one of the constants in this class.
   */
  public function getStatus() {
    return $this->status;
  }

  /**
   * Set the activity states.
   *
   * @param $activityStates
   *   An array of active activity states.
   * TODO: Support the different types of required states.
   */
  public function setActivityStates($activityStates) {
    $this->activityStates = $activityStates;
  }

  /**
   * Set an individual activity state.
   *
   * @param $activityState
   *   A single active activity state object.
   */
  public function setActivityState(ConductorActivityState $activityState) {
    $this->activityStates[$activityState->name] = $activityState;
  }

  public function setStorage(ConductorStorage $storageHandler) {
    $this->storageHandler = $storageHandler;
  }

  /**
   * Get the storageHandler for this instance (instantiating the configured default if none is set).
   */
  public function getStorage() {
    if (is_null($this->storageHandler)) {
      $this->storageHandler = conductor_get_storage_handler(variable_get('conductor_default_storage_handler', 'database'));
    }
    return $this->storageHandler;
  }

  /**
   * Get an activity state.
   *
   * @param $name
   *   The name of the activity in the workflow.
   *
   * @return
   *   A ConductorActivityState descended object.
   */
  public function getActivityState($name = NULL) {
    $return = FALSE;

    if (is_null($name)) {
      $return = $this->activityStates;
    }
    else if(isset($this->activityStates[$name])) {
       $return = $this->activityStates[$name];
    }

    return $return;
  }

  /**
   *
   */
  public function getIntanceID() {
    // TODO: we need to generate unique ID's here.  The specifics will vary
    // depending on the storage hanlder, it needs to be called to get this...
    return drupal_hash_base64(drupal_random_bytes(55));
  }

  /**
   * Retrieve the current active activities.
   *
   * @return
   *   A linear array of active activities.
   */
  public function getActiveActivities() {
    // If we have neither active nor completed activities, activate the first
    // activities.
    // TODO: Should this happen automatically? or should this always be
    // explicitly called on by the activity starter?
    if (!$this->countBin('active') && !$this->countBin('complete') && !$this->countBin('failed')) {
      $this->activateActivity($this->workflow->getFirstActivity());
    }
    return $this->getBinItems('active');
  }

  /**
   * Retrieve the current failed activities.
   *
   * @return
   *   A linear array of failed activities.
   */
  public function getFailedActivities() {
    return $this->getBinItems('failed');
  }

  /**
   * Retrieve the current suspended activities.
   *
   * @return
   *   A linear array of suspended activities.
   */
  public function getSuspendedActivities() {
    return $this->getBinItems('suspended');
  }

  /**
   * Get a reference to an array holding a bin of related activity names.
   *
   * @param $binName
   *   A string representing this logical set of activity states.
   * @return
   *   A linear array of activity names.
   */
  private function &getBin($binName) {
    if (!isset($this->activityBins[$binName])) {
      $this->activityBins[$binName] = array();
    }
    return $this->activityBins[$binName];
  }


  /**
   * Get the number of items in a bin.
   *
   * @param $binName
   *   A string representing this logical set of activity states.
   * @param $activity
   *   A ConductorActivity descended object.
   */
  private function addToBin($binName, ConductorActivity $activity) {
    $bin = &$this->getBin($binName);
    if (!in_array($activity->name, $bin)) {
      $bin[] = $activity->name;
    }
  }

  /**
   * Check to see whether an activity is in a bin.
   *
   * @param $binName
   *   A string representing this logical set of activity states.
   * @param $activity
   *   A ConductorActivity descended object.
   * @return
   *   Boolean.
   */
  private function checkInBin($binName, ConductorActivity $activity) {
    $bin = &$this->getBin($binName);
    return in_array($activity->name, $bin);
  }

  /**
   * Get the number of items in a bin.
   *
   * @param $binName
   *   A string representing this logical set of activity states.
   * @return
   *   An integer representing the number of items in a bin.
   */
  private function countBin($binName) {
    return count($this->getBin($binName));
  }

  /**
   * Remove an activity from a bin.
   *
   * @param $binName
   *   A string representing this logical set of activity states.
   * @param $activity
   *   A ConductorActivity descended object.
   * @return
   *   Boolean, true if it was removed from the bin, false if it wasn't in the
   *   bin to begin with.
   */
  private function removeFromBin($binName, ConductorActivity $activity) {
    $bin = &$this->getBin($binName);
    $position = array_search($activity->name, $bin);
    if ($position === FALSE) {
      return FALSE;
    }
    unset($bin[$position]);
    return TRUE;
  }

  /**
   * Get state objects associated with the activities listed in a bin.
   *
   * @param $binName
   *   A string representing this logical set of activity states.
   * @return
   *   A linear array of ConductorActivityState descended objects from the bin.
   */
  private function getBinItems($binName) {
    $bin = &$this->getBin($binName);
    $activities = array();
    foreach ($bin as $activity_name) {
      if (isset($this->activityStates[$activity_name])) {
        $activities[$activity_name] = &$this->activityStates[$activity_name];
      }
    }
    return $activities;
  }

  /**
   * Activate a specified activity.
   *
   * @param $activity
   *   The ConductorActivity that has successfully been activated.
   * @param $state
   *   An ConductorActivityState object represeting the state of the present activity.
   *
   *  TODO: Obviously this second parameter has been removed raising the
   *  question; how do you use the non-default activity state?  Each activity
   *  can override the state getter method to use a different one, but should we
   *  allow dependency injection for different ones? for different ones for
   *  different types?  For different ones for different instances?
   */
  public function activateActivity(ConductorActivity $activity) {
    if (!$this->checkInBin('active', $activity) && !$this->checkInBin('failed', $activity) && !$this->checkInBin('complete', $activity)) {
      // Instantiate the appropriate state handler if we do not already have one.
      if (!isset($this->activityStates[$activity->name])) {
        $this->activityStates[$activity->name] = &$activityState;
      }
      $activityState = $activity->getState();
      // Currently an activity needs to have the state object for each of its inputs.
      foreach ($activity->inputs as $activityName) {
        if (isset($this->activityStates[$activityName])) {
          $activityState->setInputActivityState($this->activityStates[$activityName]);
        }
      }
      if ($activity->checkRunnability()) {
        $this->notifyObservers('activityActivate', $activity);
        $this->removeFromBin('suspended', $activity);
        $this->addToBin('active', $activity);
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
  }

  /**
   * Deactivate an activity in the workflow.
   *
   * This removes the activity from the list of candidates for execution.
   */
  public function deactivateActivity(ConductorActivity $activity) {
    $this->removeFromBin('active', $activity);
    $this->notifyObservers('activityDeactivate', $activity);
  }

  /**
   * Suspend an activity.
   */
  public function suspendActivity(ConductorActivity $activity) {
    if (!$this->checkInBin('suspended', $activity)) {
      $this->addToBin('suspended', $activity);
      $this->removeFromBin('active', $activity);
      $this->notifyObservers('activitySuspend', $activity);
    }
  }

  /**
   *
   */
  public function failActivity(ConductorActivity $activity) {
    $activity->getState()->setStatus(ConductorActivityState::FAILED);
    $this->removeFromBin('active', $activity);
    $this->addToBin('failed', $activity);
    $this->notifyObservers('activityFailed', $activity);
  }

  /**
   * Get a list of the completed activities.
   */
  public function getCompletedActivities() {
    return $this->getBinItems('complete');
  }

  /**
   * Complete a specified activity.
   *
   * @param $activity
   *   The ConductorActivity that has successfully been run and reported finished and should be marked complete.
   */
  public function completeActivity(ConductorActivity $activity) {
    $this->removeFromBin('active', $activity);
    $this->addToBin('complete', $activity);
    foreach ($activity->outputs as $activity_name) {
      $output = &$this->workflow->getActivity($activity_name);
      $this->activateActivity($output);
    }
  }

  /**
   * This workflow could not be completed at this time and has been suspended for later resuming.
   */
  public function workflowSuspended() {
    $this->status = self::SUSPENDED;
  }
  /**
   * This workflow is complete.
   */
  public function workflowComplete() {
    $this->status = self::FINISHED;
    if ($this->uniqueId) {
      $this->getStorage()->delete($this->uniqueId);
    }
  }

  /**
   * This workflow is complete.
   */
  public function workflowFailed() {
    $this->status = self::FAILED;
  }

  /**
   * Suspend this workflow because it is awaiting some other input.
   *
   * This method will persist the database using this object's storage handler.
   */
  public function suspend() {
    $pointers = array();
    $state = new stdClass;
    if ($this->uniqueId !== FALSE) {
      $state->uniqueId = $this->uniqueId;
    }
    $state->activityBins = $this->activityBins;
    $state->activityStates = $this->getActivityState();
    $state->status = $this->status;
    $state->uniqueId = $this->getStorage()->save($state);
    foreach ($this->getSuspendedActivities() as $name => $suspendedActivity) {
      if ($pointerSet = $this->workflow->getActivity($name)->getSuspendPointers()) {
        foreach ($pointerSet as $pointerKey) {
          $this->getStorage()->savePointer($this->workflow->name, $state->uniqueId, $name, $pointerKey);
        }
      }
    }
  }

  /**
   * Resume a suspended workflow.
   */
  public function resume(array $context = array()) {
    // TODO: This is some fugly program flow and what if I want to
    // resume a workflow but not run it just yet?
    $this->notifyObservers('workflowResumed', $this);
    $this->status = self::RUNNING;
    $workflow = $this->workflow;
    foreach ($context as $pointerKey => $context) {
      $pointer = $this->getStorage()->loadPointer($pointerKey);
      // TODO: fix inconsistency between unique id and instance id.
      $this->loadFromUniqueId($pointer['instanceId']);
      $activity = $workflow->getActivity($pointer['activityName']);
      $this->activateActivity($activity);
      foreach ($context as $name => $value) {
        $activity->getState()->setContext($name, $value);
      }
      if ($activity->checkRunnability()) {
        $this->getStorage()->deletePointer($pointerKey);
      }
    }
    $workflow->run();
  }

  /**
   * Load a suspended workflow instance from its unique id.
   */
  public function loadFromUniqueId($instanceId) {
    $state = $this->getStorage()->load($instanceId);
    if ($state) {
      $this->activityBins = $state->activityBins;
      $this->activityStates = $state->activityStates;
      $this->status = $state->status;
      $this->uniqueId = $state->uniqueId;
      // Reference each activity in the workflow to the state retrieved from storage.
      foreach ($this->activityStates as $activityState) {
        $this->workflow->getActivity($activityState->name)->setState($activityState);
      }
    }
    else {
      throw new Exception(t('Workflow instance ID @id not found.', array('@id' => $instanceId)));
    }
  }


  /**
   * Register an observer.
   *
   * @param $observer
   *   A new ConductorObserver descended object.
   */
  public function registerObserver($observer) {
    $this->observers[] = $observer;
  }

  /**
   * Notify all registered observers of an event that occured.
   *
   * @param $event
   *   A camelcase string identifying the name of the event that occured.
   * @param $object
   *   The conductor object that 
   */
  public function notifyObservers($event, $object) {
    if ($this->observers) {
      foreach ($this->observers as $observer) {
        $observer->notify($event, $object);
      }
    }
  }

  /**
   * Set context for this instance.
   *
   * If one parameter is given and it is an array, reset the context to that array.
   * if two parameters are given, set them as a key => value pair to the existing
   * context array.
   *
   * @param $one
   *   If two is present the key of the context value to set, if two is not
   *   present then an array to set this context to.
   * @param $two
   *   The value of the context value to set.
   */
  public function setContext($one, $two = NULL) {
    $args = func_get_args();
    if (count($args) == 2) {
      $this->context[$one] = $two;
    }
    else if (count($args) == 1 && is_array($one)) {
      $this->context = $one;
    }
    else {
      throw new Exception('Invalid context provided to workflow instance in method `setContext`.');
    }
  }

  /**
   * Get the context for this instance.
   */
  public function getContext($name = FALSE) {
    if (!$name) {
      $return = $this->context;
    }
    else {
      if (isset($this->context[$name])) {
        $return = $this->context[$name];
      }
      else {
        $return  = FALSE;
      }
    }
    return $return;
  }

}
