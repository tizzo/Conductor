<?php

/**
 * This class maintains the state of a Conductor workflow.
 *
 * ConductorState is handed into a ConductorWorkflow to track the state of a specific
 * instance of that workflow.  It delegates saving to a member object stored in the
 * $storageHandler attribute, which must implement the ConductorStorage interface.
 *
 * ConductorState is comprised of ConductorActivityState descended objects.
 * Each of these maintains the state of an individual activity.
 */
class ConductorState {

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
   * An array of CondcutorActivityState descended object.
   *
   * Each of these objects tracks the state for a specific activity in the
   * workflow.
   */
  protected $activityStates = array();

  /**
   * An array of active activities.
   */
  protected $activeActivities = array();

  /**
   * An array of completed activities.
   */
  protected $completedActivities = array();

  /**
   * A reference to the parent workflow.
   */
  public $workflow = NULL;

  /**
   * 
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
   * Constructor for ConductorState.
   *
   * TODO: Right now this constructor can't be used practically but it
   * should make resuming working processing dead simple when loading
   * from a persistence layer.
   *
   * @param $activityStates (optional)
   *   An associative array of ConductorActivityState objects keyed by $activity->name.
   */
  public function __construct(array $activityStates = array()) {
    $this->activityStates = $activityStates;
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

  public function setActivityState($activityState) {
    $this->activityStates[$activityState->name] = $activityState;
  }

  /**
   * Get an activity state.
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

  public function getIntanceID() {
    // TODO: we need to generate unique ID's here.  The specifics will vary
    // depending on the state class.  Perhaps we do need plugins for this
    // object?
    return drupal_hash_base64(drupal_random_bytes(55));
  }

  /**
   * Retrieve the current active activities.
   *
   * @return
   *   A linear array of active activities.
   *   
   */
  public function getActiveActivities() {
    // If we do not yet have any activeActivities and we do not yet have any completed
    // activites, we must be starting a workflow.  Run the `start` activity.
    if (!count($this->activeActivities) && !count($this->completedActivities)) {
      $activity = $this->workflow->getActivity('start');
      if (!isset($this->activityStates['start'])) {
        $state = $this->getActivityStateHandler($activity);
      }
      else {
        $state = $this->getActivityState('start');
      }
      $this->activateActivity($activity, $state);
    }
    return $this->activeActivities;
  }

  /**
   * TODO: figure out if these are the right 
   */
  private function addToBin($bin, ConductorActivity $activity) {
    // Should we notify observers?
    //$this->notifyObservers($bin . 'Activity', $activity);
  }

  private function removeFromBin($bin, ConductorActivity $activity) {
    
  }

  private function getBin($bin) {

  }

  /**
   * Activate a specified activity.
   *
   * @param $activity
   *   The ConductorActivity that has successfully been activated.
   * @param $state
   *   An ConductorActivityState object represeting the state of the present activity.
   */
  public function activateActivity(ConductorActivity $activity, ConductorActivityState $state) {
    if (!isset($this->activeActivities[$activity->name])) {
      $this->notifyObservers('activateActivity', $activity);
      // Instantiate the appropriate state handler if we do not already have one.
      if (!isset($this->activityStates[$activity->name])) {
        $this->activityStates[$activity->name] = $this->getActivityStateHandler($activity);
      }
      $this->activeActivities[$activity->name] = &$this->activityStates[$activity->name];
    }
  }

  /**
   * Complete a specified activity.
   *
   * @param $activity
   *   The ConductorActivity that has successfully been run and reported finished and should be marked complete.
   * @param $state
   *   An ConductorActivityState object represeting the state of the present activity.
   */
  public function completeActivity(ConductorActivity $activity, ConductorActivityState $state) {
    $this->notifyObservers('completeActivity', $activity);
    $this->completedActivities[$activity->name] = &$state;
    foreach ($activity->outputs as $activity_name) {
      $activateActivity = $this->workflow->getActivity($activity_name);
      $this->activateActivity($activateActivity, $this->getActivityStateHandler($activateActivity));
    }
    $activeActivities = &$this->activeActivities;
    unset($this->activeActivities[$activity->name]);
  }

  /**
   * Factory method for ConductorActivityState descended objects.
   */
  public function getActivityStateHandler(ConductorActivity $activity) {
    // TODO: Figure out what we need to do to get the Conductor Activity State handler.
    // The actual state handler should probably be instantiated by the activity allowing
    // it to specify a different state handler.
    $activityState = new ConductorActivityState($activity, $this);
    foreach ($activity->inputs as $activityName) {
      if (isset($this->activityStates[$activityName])) {
        $activityState->setInputActivityState($this->activityStates[$activityName]);
      }
    }
    return $activityState;
  }

  /**
   * This workflow is complete.
   */
  public function workflowComplete() {
    $this->status = self::FINISHED;
  }

  /**
   * Register a new observer.
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
}
