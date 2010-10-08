// $Id$

/**
 * Add the sweaver bar at the bottom of the theme
 */

(function ($) {

Drupal.Sweaver = Drupal.Sweaver || {};

Drupal.Sweaver.types = new Array(); // A type groups different properties.
Drupal.Sweaver.properties = new Array(); // The actual css properties.
Drupal.Sweaver.selectors = new Array(); // The list of defined selector objects.
Drupal.Sweaver.css = new Object(); // Object with all targets and their properties.
Drupal.Sweaver.path = new Array(); // Full path to the root of the document.
Drupal.Sweaver.pathIndexes = new Array(); // An array with the indexes of all selected items.
Drupal.Sweaver.activePath = ''; // Currently active path.
Drupal.Sweaver.activeElement = new Object(); // Currently active element.
Drupal.Sweaver.updateMode = true; // should the form updates be saved in css?
Drupal.Sweaver.changesboxcheck = false; // Changes box check.

/**
 * Hook onload behavior
 */
$(document).ready(function() {

  Drupal.Sweaver.init();

  Drupal.Sweaver.writeCss();

  Drupal.Sweaver.changed = false;

  Drupal.Sweaver.addSliders();

  Drupal.Sweaver.addColorPicker();

  Drupal.Sweaver.updateForm();

  Drupal.Sweaver.bindClicks();

});

$(document).bind('updateCSS', function(event) {
  Drupal.Sweaver.sweaver_plugin_editor_updateCss();
});

/**
 * Implementation of HOOK_updateCss().
 *
 * Return editor css.
 */
Drupal.Sweaver.sweaver_plugin_editor_updateCss = function() {
  var css = '';
  var fullCss = '';
  var cssContent = '';

  for (var key in Drupal.Sweaver.css) {
    var target = Drupal.Sweaver.css[key];
    for (var prop in target) {
      if (Drupal.Sweaver.properties[prop]) {

        var properties = Drupal.Sweaver.properties[prop]['property'].split(' ');
        $.each(properties, function(i, property) {
          // Don't write anything if the value is empty.
          // 0 is not empty!
          if (target[prop] == '' && target[prop] != '0') {
            cssContent += '';
          }
          // Don't add a prefix and suffix for these exceptions.
          else if ((property == 'background-color' && target[prop] == 'transparent') || (property == 'background-image' && target[prop] == 'none')) {
            cssContent += '  '+ property + ': ' + target[prop] + ';\n';
          }
          else {
            cssContent += '  '+ property + ': ' + Drupal.Sweaver.properties[prop].prefix + target[prop] + Drupal.Sweaver.properties[prop].suffix + ';\n';
          }
        });
      }
    }

    if (cssContent != '') {
      css += key + '{\n';
      css += cssContent;
      css += '}\n';
      fullCss += css;
      css = '';
      cssContent = '';
    }
    // Remove key from Drupal.Sweaver.css
    else {
      delete Drupal.Sweaver.css[key];
    }
  }

  // Store css in hidden field in save form
  $("#sweaver #edit-css").val($.toJSON(Drupal.Sweaver.css));

  // Add inline css
  $("#sweaver-form #edit-css").val(fullCss);

  Drupal.Sweaver.CSSdata += fullCss;
}

/**
 * Initialize member variables and properties.
 */
Drupal.Sweaver.init = function() {

  // Get previously stored information or create empty object with all targets
  db_css = $("#edit-css");
  if (db_css.val() && db_css.val() != '[]'){
    Drupal.Sweaver.css = $.evalJSON(db_css.val());
    db_css.val('');
  }

  // Get Sweaver selectors.
  Drupal.Sweaver.selectors = Drupal.settings.sweaver['selectors'];

  // Get Sweaver types.
  Drupal.Sweaver.types = Drupal.settings.sweaver['types'];

  // Get Sweaver properties.
  Drupal.Sweaver.properties = Drupal.settings.sweaver['properties'];

  // Get classes that will never be used in the paths or generated css.
  Drupal.Sweaver.excludeClasses = Drupal.settings.sweaver['exclude_classes'];

  // Add a link popup to be able to follow links.
  $('body').append('<a href="" id="follow-link">' + Drupal.t('Click here to follow this link') + '</div>');
}

/**
 * Get all css values and update the form
 */
Drupal.Sweaver.updateForm = function() {

  // Empty form values and hide unnecessary fields
  Drupal.Sweaver.initForm();

  // Prevent changes from being saved
  Drupal.Sweaver.updateMode = false;

  // Update form with saved settings
  if (Drupal.Sweaver.activePath != '') {
    if ($("#tab-sweaver_plugin_editor").hasClass('active-tab')) {
      $("#sweaver_plugin_editor #sweaver-editor").show();
    }
    var target = '';
    if (!isEmpty(Drupal.Sweaver.activeElement)) {
      var type = Drupal.Sweaver.activeElement.type;
      if (Drupal.Sweaver.types[type]) {
        $.each(Drupal.Sweaver.types[type], function (index, object){
          if (Drupal.Sweaver.properties[object]) {
            var properties = Drupal.Sweaver.properties[object]['property'].split(' ');
            var tempValue = '';
            var value = '';
            $.each(properties, function(i, property) {
              value = $(Drupal.Sweaver.activePath).css(property);
              if (tempValue == '') {
                tempValue = value;
              }
              else {
                if (tempValue != value) {
                  value = '';
                  return false;
                }
              }
            });
            if(value != '' && !isEmpty(Drupal.Sweaver.properties[object]) && Drupal.Sweaver.properties[object].type == 'color') {
              $('#' + object + ' .colorSelector div').css('backgroundColor', value);
            }
            else if (value && !isEmpty(Drupal.Sweaver.properties[object]) && Drupal.Sweaver.properties[object].type == 'image') {
              // Remove the url() from around the image url.
              // Mozilla browsers wrap in url(""), while webkit browsers wrap in url()
              // so we need two replacements.
              stripped = value.replace('url("', '').replace('")', '').replace('url(', '').replace(')', '');
              $("#sweaver_plugin_editor #edit-" + object).val(stripped);
            }
            else {
              if (value) {
                $("#sweaver_plugin_editor #edit-" + object).val(value.replace('px', ''));
              }
            }
          }
        });
      }
    }
    Drupal.Sweaver.updateSliders();
  }
  Drupal.Sweaver.updateMode = true;
}

/**
 * Empty form values and hide unnecessary fields
 */
Drupal.Sweaver.initForm = function() {

  // Hide all sliders, all groups and all containers.
  Drupal.Sweaver.hideOverlays();
  $('#sweaver-editor .sweaver-group').hide();
  $('#sweaver-editor .container').hide();

  if (!isEmpty(Drupal.Sweaver.activeElement)) {
    // Decide which items should be shown or hidden.

    var type = Drupal.Sweaver.activeElement.type;
    $.each(Drupal.Sweaver.properties, function(index, object){
      if(object.name in Drupal.Sweaver.types[type]) {
        $('#sweaver #edit-' + object.name + '-wrapper').show();
        // From the moment that we have an visible element in a group, we need to show that group.
        $('#sweaver #edit-' + object.name + '-wrapper').parents('.sweaver-group').show();
        // From the moment that we have an visible element in a container, we need to show that container.
        $('#sweaver #edit-' + object.name + '-wrapper').parents('.container').show();
      } else {
        $('#sweaver #edit-' + object.name + '-wrapper').hide();
      }
    });
  }
}

/**
 * Show colorPicker and hook events to it
 */
Drupal.Sweaver.addColorPicker = function() {
  $('#sweaver .colorSelector').each(function() {
    var object = $(this);
    var property = object.parent().attr('id');
    object.ColorPicker({
      color: '#ffffff',
      // Determine the current color and send it to colorpicker.
      onBeforeShow: function () {
        var current_color_object = {};
        var current_color_value = ($('div', this).css('background-color')).replace('rgba(', '').replace('rgb(', '').replace(')', '').split(',');
        if (current_color_value[0] != 'transparent') {
          current_color_object.r = current_color_value[0];
          current_color_object.g = current_color_value[1];
          current_color_object.b = current_color_value[2];
          $(this).ColorPickerSetColor(current_color_object);
        }
        else {
          current_color_object.r = '255';
          current_color_object.g = '255';
          current_color_object.b = '255';
          $(this).ColorPickerSetColor(current_color_object);
        }
      },
      onShow: function (colpkr) {
        $(colpkr).fadeIn(500);
        if (object.parents('.sweaver-group-content').length == 0) {
          Drupal.Sweaver.hideOverlays();
        }
        return false;
      },
      onHide: function (colpkr) {
        $(colpkr).fadeOut(500);
        return false;
      },
      onChange: function (hsb, hex, rgb) {
      var preview = hex;
      if (hex != 'transparent') {
        preview = '#'+ hex;
      }
        $('div', object).css('backgroundColor', preview);
        if (Drupal.Sweaver.updateMode) {
          Drupal.Sweaver.setValue(property, hex);
        }
      }
    });
  });
}

/*
 * Add sliders through jQuery UI
 */
Drupal.Sweaver.addSliders = function() {
  $("#sweaver .slider-value").each(function() {
    $(this).after('<div class="slider-wrapper"><div id="' + $(this).attr('id').substr(5, $(this).attr('id').length - 5) + '-slider" class="slider"></div></div>');
  });

  $("#sweaver .slider-value").click(function() {
    $(this).siblings('.slider-wrapper').children().slider("moveTo", $(this).val());
  });

  $("#sweaver .slider").each(function() {
    minSlider = Drupal.Sweaver.properties[$(this).attr('id').replace('-slider', '')].slider_min;
    if (minSlider == null) {
      minSlider = 0;
    }
    maxSlider = Drupal.Sweaver.properties[$(this).attr('id').replace('-slider', '')].slider_max;
    if (maxSlider == null) {
      maxSlider = 2000;
    }
    $(this).slider({
      min: minSlider,
      max: maxSlider,
      slide: function(event, ui) {
        $('#edit-' + $(this).attr("id").replace('-slider','')).val(ui.value);
        // TODO: can I trigger onchange event here?
        if (Drupal.Sweaver.updateMode) {
          Drupal.Sweaver.setValue($(this).attr("id").replace('-slider',''), ui.value);
        }
      }
    });
  });
}

/*
 * Update sliders value
 */
Drupal.Sweaver.updateSliders = function() {
  $('#sweaver .slider').each(function() {
    var object = $(this);
    name = object.attr('id').substr(0, object.attr('id').length - 7);
    val = $("#edit-" + name).val() == '' ? 0 : $("#edit-" + name).val();
    $(object).slider("moveTo", val);
  });
}

/**
 * Loop through all clickable area's and bind click
 */
Drupal.Sweaver.bindClicks = function() {

  // Get a list of selectors to exclude.
  var excludes = Drupal.settings.sweaver['exclude_selectors'];

  // Add hover outline object.
  $('#sweaver-frontend').append('<div style="position: absolute; top: 0; left: 0; border: 2px dotted #ccc" id="#sweaver-hover"></div>');

  // Never select the editor form as themeable.
  // This function appears to be needed, although we exclude on the click bind too.
  $(excludes).click(function(event) {
    event.stopPropagation();
  });

  // Build an object with all the elements that can be hovered/clicked
  var tempSelectors = $('body').find('*').filter(':parents(' + excludes + '):not(' + excludes + ')');

  // When an element is hovered, add a class 'sweaver-hovered'.
  if (Drupal.settings.sweaver['preview_selector']) {
    tempSelectors
    .bind('mouseenter', function(event){
      // Only do something when the content area is visible.
      if (Drupal.Sweaver.visible()) {
        tempObject = $(event.target);
        object = Drupal.Sweaver.buildSweaverObject(tempObject);
        // Loop through the selectors to see if the current item should be selectable.
        if (!object.translation[0]) {
          $.each(tempObject.parents(), function() {
            tempObject = $(this);
            object = Drupal.Sweaver.buildSweaverObject(tempObject);
            if (object.translation[0]) {
              return false;
            }
          });
        }
        // Make sure only one item has the outline.
        $('.sweaver-hovered').removeClass('sweaver-hovered');

        // Don't add the class on elements that cover the entire screen
        // since that would add a, annoying horizontal scrollbar.
        if (tempObject.outerWidth() != $(window).width()) {
          tempObject.addClass('sweaver-hovered');
        }
      }
    })
    .bind('mouseleave', function(event){
      // Loop through the selectors to see if the current item should be selectable.
      if (Drupal.Sweaver.visible()) {
        tempObject = $(this);
        tempObject.removeClass('sweaver-hovered');

        $.each(tempObject.parents(), function() {
          tempObject = $(this);
          object = Drupal.Sweaver.buildSweaverObject(tempObject);
          if (object.translation[0]) {
            return false;
          }
        });
        if (tempObject.outerWidth() != $(window).width()) {
          tempObject.addClass('sweaver-hovered');
        }
      }
    });
  }

  // When an element is clicked, add a class and build the entire path.
  tempSelectors
  .bind('click', function (event) {
    event.stopPropagation();
    // Only do something when the content area is visible.
    if (Drupal.Sweaver.visible()) {

      tempObject = $(event.target);
      object = Drupal.Sweaver.buildSweaverObject(tempObject);

      // Only do something if the clicked item is found in the selectors.
      if (!object.translation[0]) {
        $.each(tempObject.parents(), function() {
          tempObject = $(this);
          object = Drupal.Sweaver.buildSweaverObject(tempObject);
          if (object.translation[0]) {
            return false;
          }
        });
      }

      // clear the old paths.
      $('#sweaver_plugin_editor .sweaver-header').html('<div id="full-path" class="clearfix"></div><div id="selected-path" class="clearfix"></div>');

      $('#follow-link').hide();

      // Don't do anything if the clicked object is the 'follow-link' link.
      if (object.id != 'follow-link') {
        // If the clicked object is a link, or an element in a link, prevent default behavior.
        if(object.tag == 'a' || tempObject.parents('a').length > 0) {
          var position = tempObject.offset();
          var clickObject = tempObject;
          if (object.tag != 'a') {
            clickObject = tempObject.parents('a');
          }
          $('#follow-link').attr('href', clickObject.attr('href')).css({'top' : position.top + clickObject.outerHeight() + 5, 'left': position.left}).fadeIn();
          event.preventDefault();
        }

        // Initial check for the changesbox.
        if (Drupal.Sweaver.changesboxcheck == false) {
          Drupal.Sweaver.ChangesBox(true);
          Drupal.Sweaver.changesboxcheck = true;
        }
        
        // Reset some values.
        Drupal.Sweaver.path.length = 0;
        Drupal.Sweaver.pathIndexes.length = 0;
        $("#selected-path").html('');
        $("#full-path").html('');

        // Build path with parents.
        Drupal.Sweaver.buildPath(tempObject);
        Drupal.Sweaver.updateScreen();
        Drupal.Sweaver.updateForm();
      }
    }
  });

  // Toggle changes area.
  $('#changes-toggler').click(function(event){
    event.stopPropagation();
    Drupal.Sweaver.ChangesBox(false);
  });

  // Hide sliders and close groups when clicking outside of them.
  $("#sweaver").click(function() {
    Drupal.Sweaver.hideOverlays();
  });

  // Update css when something is changed in the form.
  $("#sweaver_plugin_editor input[id^=edit-], #sweaver_plugin_editor select[id^=edit-]").change(function(){
    if (Drupal.Sweaver.updateMode) {
      Drupal.Sweaver.setValue($(this).attr('name'), $(this).val());
    }
  });

  // Show the slider when a numeric value is entered.
  $("#sweaver_plugin_editor  .slider-value").click(function(event){
    event.stopPropagation();
    slider = $(this).siblings('.slider-wrapper');

    if (slider.is(':visible')) {
      // Close slider again on second click.
      slider.css({'visibility' : 'hidden'});
    }
    else {
      // Hide all other sliders.
      $('#sweaver_plugin_editor .slider-wrapper').css({'visibility' : 'hidden'});

      var left = -(slider.width() / 2) + ($(this).outerWidth() / 2);
      if (slider.siblings('label').is(':visible')) {
        left += slider.siblings('label').width();
      }
      var top = slider.outerHeight();
      slider.css({'left' : left, 'top' : top}).css({'visibility' : 'visible'});
    }
  });
}

/**
 * Loop through all clickable area's and bind click
 */
Drupal.Sweaver.updateScreen = function() {
  if (Drupal.settings.sweaver['preview_selector']) {
    // Add border around selected element.
    var excludes = Drupal.settings.sweaver['exclude_selectors'];
    $('.sweaver-clicked').removeClass('sweaver-clicked');
    if ($(Drupal.Sweaver.activePath).outerWidth() != $(window).width()) {
      $(Drupal.Sweaver.activePath).filter(':parents(' + excludes + '):not(' + excludes + ')').addClass('sweaver-clicked');
    }
  }
}

/**
 * Store the parents of a clicked item.
 */
Drupal.Sweaver.buildPath = function(object) {
  var index = 0;

  // Collect info on currently active item.
  Drupal.Sweaver.activeElement = Drupal.Sweaver.buildSweaverObject(object);

  // Add active element to first element in the path array.
  Drupal.Sweaver.path[0] = new Object({'id' : Drupal.Sweaver.activeElement.id, 'classes' : Drupal.Sweaver.activeElement.classes, 'tag' : Drupal.Sweaver.activeElement.tag,  'type' : Drupal.Sweaver.activeElement.type, 'translation' : Drupal.Sweaver.activeElement.translation, 'css' : Drupal.Sweaver.activeElement.css});

  // Show the currenty active path and the full path.
  Drupal.Sweaver.addToFullPath(index, true);
  Drupal.Sweaver.addToActivePath(0);

  // Traverse all parents and save them in the path array.
  var i = 1;
  var active;
  object.parents().each(function() {

    active = false;
    var parent = Drupal.Sweaver.buildSweaverObject($(this));
    if (parent.translation[0]) {
      Drupal.Sweaver.path[i] = new Object({'id' : parent.id, 'classes' : parent.classes, 'tag' : parent.tag, 'type' : parent.type, 'translation' : parent.translation, 'css' : parent.css});

      // If selector is tagged as 'highlight', automatically select it.
      var match = '';
      $.each(Drupal.Sweaver.selectors, function (index, selector) {
        if (selector.selector == 'sweaver_all_ids' || selector.selector == 'sweaver_all_classes' || selector.selector == 'sweaver_all_tags') {
          return false;
        }
        if (selector.selector == '#' + parent.id || selector.selector == parent.tag) {
          match = selector.selector;
           if (selector.highlight == '1') {
            active = true;
            Drupal.Sweaver.addToActivePath(i);
          }
        } else {
          $.each(parent.classes, function(index, aClass) {
            if (selector.selector == '.' + aClass) {
               match = selector.selector;
               if (selector.highlight == '1') {
                active = true;
                Drupal.Sweaver.addToActivePath(i);
                return false;
              }
            }
          });
        }
        if (match != '') {
         return false;
        }
      });

      // Add all items to the full path except for the html tag.
      if (parent.tag != 'html') {
        Drupal.Sweaver.addToFullPath(i, active);
      }
      i++;
    }
  });
  Drupal.Sweaver.printActivePath();
  $("#full-path").prepend('<span class="label">' + Drupal.t('Full path: ') + '</span>');
}

/**
 * Add one item to the full path.
 */
Drupal.Sweaver.addToFullPath = function(index, active) {
  var path_separator = '&nbsp;&gt;&nbsp;';
  var active_class = '';

  // Don't show a seperator after the last item in the path.
  if (index == 0) {
    path_separator = '';
  }

  // Add an active class to the selected items.
  if (active == true) {
    active_class = ' active';
  }

  // Get the list of translated selectors.
  var selectorList = Drupal.Sweaver.path[index].translation;

  // First add the default selector.
  $("#full-path").prepend('<div class="selector-wrapper' + active_class + '" id="sid-' + index + '"><div class="first-selector"><a title="' + Drupal.t('Click to add this element to the selected path') + '">' + selectorList[0] + '</a></div><div class="selector-separator">' + path_separator + '</div></div>');

  // Next add a popup with all possible selectors.
  if (selectorList.length > 2) {
    var tempObj = ''
    for (var i=1; i < selectorList.length; i++) {
      tempClass = '';
      // Add a class active to indicate the preferred selector.
      if (i == 1) {
        tempClass += 'active ';
      }
      if (i == 1) {
        tempClass += 'first ';
      }
      if (i == selectorList.length - 1) {
        tempClass += 'last';
      }
      tempObj += '<li class="' + tempClass + '"><a href="#" id="ssid-' + (i-1) + '">' + selectorList[i] + '</a></li>';
    }
    $("#sid-" + index).prepend('<div class="selector-popup-opener">open</div><div class="selector-popup"><ul>' +tempObj + '</ul></div>');
  }

  // Bind click to change the active path.
  $('#sid-' + index + ' .first-selector a').click(function() {
    $(this).parent().parent().toggleClass('active');
    Drupal.Sweaver.addToActivePath(index);
    Drupal.Sweaver.printActivePath();
    // Reset the active element as it might have changed.
    Drupal.Sweaver.pathIndexes.sort(function(a,b){return a - b});
    Drupal.Sweaver.activeElement = Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[0]] ? Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[0]] : {} ;
    Drupal.Sweaver.updateForm();
    Drupal.Sweaver.updateScreen();

    // Stop the link from doing anything.
    return false;
  });

  // Bind click to change the active path from a popup.
  $('#sid-' + index + ' .selector-popup a').click(function() {
    // Store in the active object that there is a new preferred selector instead of the first one defined in the backend.
    var link = $(this);
    var i = link.attr('id').substr(5);
    Drupal.Sweaver.path[index].preferredSelector = i;
    Drupal.Sweaver.printActivePath();
    link.parents('.selector-popup').hide();
    // Replace the selector in the full path.
    $('#sid-' + index + ' .first-selector a').html(link.html());
    // Add an active class.
    link.parent().siblings('.active').removeClass('active');
    link.parent().addClass('active');
    Drupal.Sweaver.updateForm();
    Drupal.Sweaver.updateScreen();
    return false;
  });

  // Bind click to hide/show selector popups.
  $('#sid-' + index + ' .selector-popup-opener').click(function() {
    var popup = $(this).siblings('.selector-popup');
    if (popup.is(':visible')) {
      popup.hide();
    }
    else {
      popup.slideDown('fast');
    }
    $('#sweaver .selector-popup').hide();
  });
}

/**
 * Add an item to the active path.
 */
Drupal.Sweaver.addToActivePath = function(i) {
  // Do not add the item when selected or remove it from Active path.
  var position = $.inArray(i, Drupal.Sweaver.pathIndexes);
  if (position < 0) {
    Drupal.Sweaver.pathIndexes.unshift(i);
  }
  else {
    // Remove from pathIndexes if necessary.
    for (var key in Drupal.Sweaver.pathIndexes) {
      if (Drupal.Sweaver.pathIndexes[key] == i) {
        Drupal.Sweaver.pathIndexes.splice(key, 1);
      }
    }
  }
}

/**
 * Print the active path.
 */
Drupal.Sweaver.printActivePath = function() {
  // Reset the previous path and add the next item to pathIndexes.
  $("#selected-path").html('');

  // Sort pathIndexes.
  Drupal.Sweaver.pathIndexes.sort(function(a,b){return a - b});

  // Print the selected path in human-readable language.
  if (Drupal.Sweaver.pathIndexes.length > 0) {
    for ( var i=0, len=Drupal.Sweaver.pathIndexes.length; i<len; ++i ){
      if (i > 0) {
        $("#selected-path").append(' in ');
      }
      // See which translation should be used.
      var j = Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]].preferredSelector ? Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]].preferredSelector : 0;
      j++;
      $("#selected-path").append(Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]].translation[j]);
    }

    // Save the currently active css path.
    Drupal.Sweaver.activePath = '';
    Drupal.Sweaver.pathIndexes.reverse();
    for (var i=0, len=Drupal.Sweaver.pathIndexes.length; i<len; ++i){
      // See which translation should be used.
      var j = Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]].preferredSelector ? Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]].preferredSelector : 0;
      j++;
      Drupal.Sweaver.activePath += Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]].css[j] + ' ';
    }
  }
  else {
    $("#selected-path").html(Drupal.t('none'));
  }
  $("#selected-path").prepend('<span class="label">' + Drupal.t('Selected item: ') + '</span>');
}

/**
 * Fill the activeElement and update the Form and ActivePath.
 * Other plugins can use this function to set values on the
 * style tab. To switch tabs, you can use the Drupal.Sweaver.switchTab
 * function.
 */
Drupal.Sweaver.updateStyleTab = function(theClass, name) {
  // Build the object with manipulated data.
  var tempObject = Drupal.Sweaver.buildSweaverObject($('.' + theClass));

  tempObject.translation = new Array(name, name);
  tempObject.classes = new Array('.' + theClass);
  tempObject.css = new Array('.' + theClass, '.' + theClass);

  Drupal.Sweaver.activeElement = tempObject;

  // Build path with parents.
  $('#sweaver_plugin_editor .sweaver-header').html('<div id="selected-path" class="clearfix"></div>');
  Drupal.Sweaver.path[0] = new Object({'id' : Drupal.Sweaver.activeElement.id, 'class' : Drupal.Sweaver.activeElement.classes, 'tag' : Drupal.Sweaver.activeElement.tag,  'type' : Drupal.Sweaver.activeElement.type, 'translation' : Drupal.Sweaver.activeElement.translation, 'css' : Drupal.Sweaver.activeElement.css});
  Drupal.Sweaver.addToFullPath(0, true);
  Drupal.Sweaver.pathIndexes = new Array();
  Drupal.Sweaver.addToActivePath(0, tempObject);
  Drupal.Sweaver.printActivePath();
  Drupal.Sweaver.activePath = '.' + theClass;
  Drupal.Sweaver.updateForm();
  Drupal.Sweaver.updateScreen();
}

/**
 * Store new value and update inline css.
 */
Drupal.Sweaver.setValue = function(property, value) {
  if (!Drupal.Sweaver.css[Drupal.Sweaver.activePath]) {
    Drupal.Sweaver.css[Drupal.Sweaver.activePath] = new Object();
  }
  Drupal.Sweaver.css[Drupal.Sweaver.activePath][property] = value;
  Drupal.Sweaver.writeCss();

  // Check for state of changes box.
  if (Drupal.Sweaver.cookie('sweaver_changes_box') == 'true') {
    Drupal.Sweaver.writeChanges();
  }
}

/**
 * Write changes to the changes popup so that people can remove them again.
 */
Drupal.Sweaver.writeChanges = function() {
  $('#editor-changes').html('');
  for (key in Drupal.Sweaver.css) {
    var target = Drupal.Sweaver.css[key];
    for (prop in target) {
      // 0 is not empty too!
      if (Drupal.Sweaver.properties[prop] && (target[prop] != '' || target[prop] == '0')) {
        // Special case for transparent.
        if ((prop == 'background-color' && target[prop] == 'transparent') || (prop == 'background-image' && target[prop] == 'none')) {
          $('#editor-changes').prepend($('<p onclick="var event = arguments[0] || window.event; event.stopPropagation(); Drupal.Sweaver.deleteProperty(\'' + key + '\', \'' + prop + '\')">' + key + ': '+ prop + ': ' + target[prop] + '</p>'));
        }
        else {
          $('#editor-changes').prepend($('<p onclick="var event = arguments[0] || window.event; event.stopPropagation(); Drupal.Sweaver.deleteProperty(\'' + key + '\', \'' + prop + '\')">' + key + ': '+ prop + ': ' + Drupal.Sweaver.properties[prop].prefix + target[prop] + Drupal.Sweaver.properties[prop].suffix + '</p>'));
        }
      }
    }
  }
}

/**
 * Delete a property from a selector.
 */
Drupal.Sweaver.deleteProperty = function(key, property) {
  var target = Drupal.Sweaver.css[key];
  Drupal.Sweaver.css[key] = {};
  for (var prop in target) {
    if (prop != property) {
      Drupal.Sweaver.css[key][prop] = target[prop];
    }
  }
  Drupal.Sweaver.writeCss();
  Drupal.Sweaver.writeChanges();
  Drupal.Sweaver.updateForm();
}

/**
 * Translate an parent item in a human-readable name.
 */
Drupal.Sweaver.objectToReadable = function(object) {

  var translation = new Array();
  var css = new Array();
  var selector = '';
  var description = '';
  var tempSelectors = new Array();

  // Traverse all selectors defined in the backend and return an array with the description.
  // TODO: move translation.push en tempSelectors to seperate function.
  $.each(Drupal.Sweaver.selectors, function() {
    selector = this.selector;
    name = this.name;
    description = this.description;

    if (name == 'allids') {
      if (object.id && $.inArray('#' + object.id, tempSelectors) < 0) {
        translation.push('the ' + object.id + ' region');
        css.push('#' + object.id);
        tempSelectors.push('#' + object.id);
      }
    }
    else if (name == 'allclasses') {
      if (object.classes && object.classes[0]) {
        $.each(object.classes, function(index, tempClass) {
          if ($.inArray(tempClass, Drupal.Sweaver.excludeClasses) < 0 && $.inArray('.' + tempClass, tempSelectors) < 0) {
            translation.push('all ' + tempClass);
            css.push('.' + tempClass);
            tempSelectors.push('.' + tempClass);
          }
        });
      }
    }
    else if (name == 'alltags' && $.inArray(object.tag, tempSelectors) < 0) {
      translation.push(object.tag);
      css.push(object.tag);
      tempSelectors.push(object.tag);
    }
    else {
      if (selector == '#' + object.id && $.inArray('#' + object.id, tempSelectors) < 0) {
        translation.push(description);
        css.push('#' + object.id);
        tempSelectors.push('#' + object.id);
      } else if (selector == object.tag && $.inArray(object.tag, tempSelectors) < 0) {
        translation.push(description);
        css.push(object.tag);
        tempSelectors.push(object.tag);
      } else {
        $.each(object.classes, function(index, tempClass) {
          if (selector == '.' + tempClass  && $.inArray(tempClass, Drupal.Sweaver.excludeClasses) < 0 && $.inArray('.' + tempClass, tempSelectors) < 0) {
            translation.push(description);
            css.push('.' + tempClass);
            tempSelectors.push('.' + tempClass);
          }
        });
      }
    }
  });

  // If a prefered selector was set in the object, return that one instead of the default first one.
  index = object.preferredSelector ? object.preferredSelector : 0;
  translation.splice(0, 0, translation[index]);
  css.splice(0, 0, css[index]);

  object.translation = translation;
  object.css = css;
  return translation;
}

/**
 * Build a Sweaver object.
 */
Drupal.Sweaver.buildSweaverObject = function(object) {
  var tempObject = new Object;
  tempObject.id = object.attr('id');
  tempObject.classes = trim(object.attr('class')).split(' ');
  tempObject.tag = object.get(0).tagName.toLowerCase();
  tempObject.type = object.css('display');

  // Fallback to block if an unknow type is detected.
  if (!(tempObject.type in Drupal.Sweaver.types)) {
    tempObject.type = 'block';
  }

  // Generate a human-readable name and a css selector.
  Drupal.Sweaver.objectToReadable(tempObject);
  return tempObject;
}

/**
 * Helper function to remove trailing leading and multiple spaces.
 */
function trim(s) {
  s = s.replace(/(^\s*)|(\s*$)/gi,"");
  s = s.replace(/[ ]{2,}/gi," ");
  s = s.replace(/\n /,"\n");
  return s;
}

/**
 * Hide all sliders.
 */
Drupal.Sweaver.hideOverlays = function() {
  $('#sweaver .slider-wrapper').css({'visibility' : 'hidden'});
}

/**
 * Open or close the changes box().
 */
Drupal.Sweaver.ChangesBox = function(cookie_check) {

  var show_box = false;
  box = $('#editor-changes');
  toggler = $('#changes-toggler');

  if (cookie_check == true && Drupal.Sweaver.cookie('sweaver_changes_box') == 'true') {
    show_box = true;
  }
  else if (parseInt(box.css('right')) < 0 && cookie_check == false) {
    show_box = true;
  }

  if (show_box == true) {
    Drupal.Sweaver.writeChanges();
    box.css({'right' : '10px'});
    toggler.addClass('open').html(Drupal.t('Hide changes'));
    Drupal.Sweaver.cookie('sweaver_changes_box', true);
  } else {
    box.css({'right' : '-10000px'});
    toggler.removeClass('open').html(Drupal.t('Show changes'));
    Drupal.Sweaver.cookie('sweaver_changes_box', false);
  }
}

/**
 * Check wether the editor tab is visible.
 */
Drupal.Sweaver.visible = function() {
  if (Drupal.Sweaver.open == 'true' && $('#sweaver_plugin_editor .sweaver-content').is(':visible')) {
    return true;
  }
  else {
    return false;
  }
}

/**
 * Helper function to check if an object is empty.
 */
function isEmpty(obj) {
  for(var prop in obj) {
    if(obj.hasOwnProperty(prop))
      return false;
  }
  return true;
}


/**
 * Add custom expression to exclude all selectors in the sweaver bar.
 */
$.expr[':'].parents = function(a,i,m){
  return jQuery(a).parents(m[3]).length < 1;
};

})(jQuery);
