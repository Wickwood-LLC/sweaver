/* $Id$ */

Drupal.behaviors.sweaverThemeSettings = function (context) {
  $('#sweaver_plugin_themesettings fieldset.collapsible > legend:not(.collapse-processed)', context).each(function() {
    var fieldset = $(this.parentNode);
    // Expand if there are errors inside
    if ($('input.error, textarea.error, select.error', fieldset).size() > 0) {
      fieldset.removeClass('collapsed');
    }

    // Turn the legend into a clickable link and wrap the contents of the fieldset
    // in a div for easier selection.
    var text = this.innerHTML;
      $(this).empty().append($('<a href="#">'+ text +'</a>').click(function() {
        var setting = $(this).parents('fieldset:first')[0].innerHTML;
        Drupal.Sweaver.setMessage(setting);
        return false;
      }))
      .after($('<div class="sweaver-themesettings-wrapper"></div>')
      .append(fieldset.children(':not(legend):not(.action)')))
      .addClass('collapse-processed');
  });
};