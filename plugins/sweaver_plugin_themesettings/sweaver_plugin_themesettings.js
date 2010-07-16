/* $Id$ */

Drupal.behaviors.sweaverThemeSettings = function (context) {
  $('#sweaver_plugin_themesettings fieldset.collapsible > legend:not(.collapse-processed)', context).each(function() {
    var fieldset = $(this).parent();
    var legend = $(this);
    // Expand if there are errors inside
    if ($('input.error, textarea.error, select.error', fieldset).size() > 0) {
      fieldset.removeClass('collapsed');
    }

    // Turn the legend into a clickable link and wrap the contents of the fieldset
    // in a div for easier selection.
    var text = legend.text();
    legend.empty()
    .append($('<a href="#">'+ text +'</a>').click(function() {
      popup = $('#sweaver-popup');
      if (popup.is(':visible')) {
        Drupal.Sweaver.hidePopup();
      }
      if (popup.is(':visible') && $(this).hasClass('open-tab')) {
        Drupal.Sweaver.hidePopup();
        $(this).removeClass('open-tab');
      }     
      else {
        var form = legend.siblings();
        $('#sweaver_plugin_themesettings .open-tab').removeClass('open-tab');        
        $(this).addClass('open-tab');
        Drupal.Sweaver.showPopup(form, $('#sweaver_plugin_themesettings .open-tab').parent().siblings('.sweaver-themesettings-wrapper'));
      }    
      return false;
    }))
    .append(fieldset.children('.description')[0].html)
    .after($('<div class="sweaver-themesettings-wrapper"></div>')
    .append(fieldset.children(':not(legend):not(.action)')))
    .addClass('collapse-processed');
  });
  $('#sweaver_plugin_themesettings .form-submit').click(function() {
    Drupal.Sweaver.hidePopup();
  })
};