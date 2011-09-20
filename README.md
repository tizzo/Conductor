# Conductor #

This module is in the very earliest stages of development and at the
time of writing is not functional.

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
is optional for workflows that do not need to be resumed.  Workflow state storage has a pluggable
storage backend allowing it, too, to be persisted to a NoSQL or in memory data store.


## Terminology ##

1. Workflow
2. Activity
3. Workflow State

### 1. Workflow ###

A workflow is a process.  These processes have different steps, generically refered to as
activities, that a particular workflow moves through.

## Storage ##

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

## Storage ##

Activities are members of the workflow object and are stored in a serialized or exported
format inside the workflow itself.  There is no external storage of activities beyond a
workflow that a given activity is a member of.


### 3. Workflow State ###

Workflow state represents a single instance of a workflow.

## Storage ##

Workflow state is stored using another ctools plugin and so storage can be swapped out.
The activities on a workflow are stored with the workflow state in their configuration
at the time that workflow is stored.  This is important because without this otherwise
changing the workflow while a particular instance is in the process of completion could
result in stranded workflow items with inconsistent state.

## About workflow processing ##

Conductor workflows are currently started off by some module other than conductor which
hands in whatever context is necessary for processing.  This becomes the bases for the
workflow state which is passed from activity to activity.  The activity can then ask
questions of the WorkflowActivityState descended object and use the context present to perform
its own activities.

When an activity is completed (always starting with the `start` activity), its name is
added to the `completedActivities` array in the WorkflowState object and each of its
specified outputs is activated making them eligible for processing.

## Updating Conductor ##

One question that needs to be answered for Conductor to be a viable option as the center
point for a site's workflow managnement, is how we are going to manage updating to a new
version of workflow with processes in the midst of processing.  At the time of writing the
API is highly unstable and we can't make any promises about whether changes will break
existing workflows.
