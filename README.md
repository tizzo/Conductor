# Conductor #

Conductor aims to allow you to easily model business processes that contain multiple steps
in a flexible way.  It is essentially a way of connecting individual components in a user
configurable way and managing the state of these workflows as they move from step to step.
This approach should be very good at providing content workflows but may also be used in
real-time data processing and most business processes that require a series of disconnected
steps.

The goal is to create a module akin to [maestro](http://drupal.org/project/maestro) but
built on top of [ctools](http://drupal.org/project/ctools) with an architecture inspired
by [views](http://drupal.org/project/views) with proper exportables support and a much
higher degree of flexibility and to allow realtime message handling, rather than requiring
later cron jobs.  The hope is to support [Rules](http://drupal.org/project/rules)
actions and conditions in the near future, enabling an enormous amount of functionality
without reinventing the wheel.

## Performance ##

Conductor is aiming at being highly performant and scalable with capabilities built in, from the
beginning, to run without hitting the database any more than it needs to.  Workflows are cached
allowing them to be loaded by name easily form in memory key-value stores.  Presistence of state
is optional for workflows that do not need to be resumed.  Workflow instance storage has a pluggable
storage backend allowing it, too, to be persisted to a NoSQL or in memory data store.

## Terminology ##

1. Workflow
2. Activity
3. Workflow Instance
4. Workflow Activity State
5. Workflow Storage

### 1. Workflow ###

A workflow is set of connected steps that are performed in a sequence that represents some
business process.  These processes different steps, generically refered to as activities,
can branch and merge and suspend themselves while awaiting external input.

#### Storage ####

Conductor workflows are implemented as [ctools exportables](http://drupalcode.org/project/ctools.git/blob_plain/7.x-1.x:/help/export.html).
This means they may be provided in code or in the database and if there is a rendition
in both code and the database the version in the database is used but may be reverted
to the version specified in code via the user interface.

Conductor workflows want to cache themselves.  Workflows in the database will be cached
so that it can be retrieved from memory on sites using [memcached](http://drupal.org/project/memcache)
without making additional database calls for processing.

### 2. Activity ###

An activity is a single step in a workflow process.
Activities are implemented as [ctools plugins](http://drupalcode.org/project/ctools.git/blob_plain/7.x-1.x:/help/plugins-api.html).

####Storage ####

Activities are members of the workflow object and are stored in a serialized or exported
format inside the workflow itself.  There is no external storage of activities beyond a
workflow that a given activity is a member of.


### 3. Workflow Instance ###

Workflow instance represents a single instance of a workflow as it is processed.

#### Storage ####

Workflow instance is stored using another ctools plugin and so storage can be swapped out.
The activities on a workflow are stored with the workflow state in their configuration
at the time that workflow is stored.  This is important because without this otherwise
changing the workflow while a particular instance is in the process of completion could
result in stranded workflow items with inconsistent state.

## About workflow processing ##

Conductor workflows are currently started off by some module other than conductor which
hands in whatever context is necessary for processing.  This becomes the bases for the
workflow instance which is passed from activity to activity.  The activity can then ask
questions of the WorkflowActivityState descended object and use the context present to perform
its own activities.

When an activity is completed (always starting with the `start` activity), its name is
added to the `completedActivities` array in the WorkflowState object and each of its
specified outputs is activated making them eligible for processing.

## Context ##

Note: the word context is probably the most overloaded word in the Drupal ecosystem.
Here "context" simply means some data relevant to the operation being performed.

Most activities will need some data to work with and some activities may load additional
data to be utilized by subsequent processes.  To properly accomodate the anticipated
context needs we will likely need to add named inputs and outputs for activities and
allow them to specify what context they consume or expect.  There is some question as
to how we can allow multiple context items to move through the chain for later use.  It
is clear that a simple array is not sufficient for our requirements and we will likely
need to make this system much more robust.

## Updating Conductor ##

One question that needs to be answered for Conductor to be a viable option as the center
point for a site's workflow managnement, is how we are going to manage updating to a new
version of workflow with processes in the midst of processing.  At the time of writing the
API is highly unstable and we can't make any promises about whether changes will break
existing workflows.
