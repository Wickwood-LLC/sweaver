/* $Id$ */

(function ($) {

$(document).bind('updateCSS', function(event) {
  Drupal.Sweaver.sweaver_plugin_customcss_updateCss();
});

/**
 * Implementation of HOOK_updateCss().
 *
 * Return custom css.
 */
Drupal.Sweaver.sweaver_plugin_customcss_updateCss = function() {
  Drupal.Sweaver.CSSdata += $('#edit-sweaver-plugin-custom-css').val();
}

/**
 * Preview button onclick behavior.
 */
Drupal.behaviors.SweaverCustomCss = {
  attach: function (context) {
    $('#edit-sweaver-plugin-custom-css-button').click(function(){
      Drupal.Sweaver.writeCss();
      Drupal.Sweaver.setMessage(Drupal.t('Your custom css has been succesfully applied.'), 5000);
      return false;
    });
  }
}

})(jQuery);