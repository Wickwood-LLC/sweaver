/* $Id$ */

Drupal.behaviors.sweaverThemeSettings = function (context) {
  $('#sweaver_plugin_themesettings .sweaver-settings-popup a', context).each(function() {

    $(this).click(function() {

      var wrapper = $(this).attr('id').replace('link', 'data');

      popup = $('#sweaver-popup');
      /*if (popup.is(':visible')) {
        Drupal.Sweaver.hidePopup();
      }*/
      if (popup.is(':visible') && $(this).hasClass('open-tab')) {
        Drupal.Sweaver.hidePopup();
        $(this).removeClass('open-tab');
      }
      else {
        $('#sweaver_plugin_themesettings .open-tab').removeClass('open-tab');
        $(this).addClass('open-tab');
        Drupal.Sweaver.showPopup($('#'+ wrapper));
      }
      return false;
    });
  });

  $('#sweaver_plugin_themesettings .form-submit').click(function() {
    Drupal.Sweaver.hidePopup();
  })
};