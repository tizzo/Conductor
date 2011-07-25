(function ($) {

Drupal.behaviors.conductor_ui = {
  attach: function (context, settings) {
    $('.conductor-ui-activity', context).draggable(); 
  }
}

})(jQuery);
