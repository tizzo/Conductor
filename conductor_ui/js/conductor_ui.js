(function ($) {

Drupal.Conductor = Drupal.Conductor || {};
Drupal.Conductor.activities = Drupal.Conductor.activities || {};

Drupal.behaviors.conductor_ui = {
  attach: function (context, settings) {
    var activities = Drupal.settings.conductor_ui.activities;
    // Iterate over each activity activating it and popuplating it with whatever
    // information we have.
    for (i in activities) {
      // Identify the js representation to use for this particular
      // activity plugin.
      var jsActivityObject = activities[i].plugin_info.conductur_ui_js_object;
      $('.' + activities[i]['css_class'], context).each(function() { Drupal.Conductor.activities[jsActivityObject](this, activities[i]) });
    }
  }
}

Drupal.Conductor.workflow = function() {
}

/**
 * A javascript representation of a base Conductor Activity.
 *
 * This is the base object that should be used by other activities as the
 * prototype to be extended.
 */
Drupal.Conductor.activities.activity = function(activityDomElement, activityInfo) {
  var x = activityInfo['x'];
  var y = activityInfo['y'];
  var inputs = {};
  var outputs = {};
  $(activityDomElement).css('top', y + 'px');
  $(activityDomElement).css('left', x + 'px');
  for (i in activityInfo) {
    //this.{activityInfo[i]} = activityInfo[i];activityInfo;
    //console.log(this.activityInfo[i]);
  }
  $(activityDomElement).click(
    function() {
    }
  );
  $(activityDomElement).draggable(
    { 
      stack: "#conductor-workflow-editor div",
      // Ideally we'd like to keep these boxes inside their parent
      // but allow the parent to grown downward.  The line below
      // does not do that.
      //containment: '#conductor-workflow-editor',
      stop: function(event, ui) {
        x = ui.position['left'];
        y = ui.position['top']
      }
    }
  );
}

/**
 * TODO: This should be moved into the plugin folder.
 */
Drupal.Conductor.activities.activityConditional = Drupal.Conductor.activities.activity;
Drupal.Conductor.activities.activityConditional.something = function() {
  // TODO: flesh this out
}

})(jQuery);

