// $Id$

/**
 * Add the sweaver bar at the bottom of the theme
 */

Drupal.Sweaver = Drupal.Sweaver || {};

Drupal.Sweaver.types = new Array(); // A type groups different properties.
Drupal.Sweaver.properties = new Array(); // The actual css properties.
Drupal.Sweaver.selectors = new Array(); // The list of defined selector objects.
Drupal.Sweaver.selectorsSelectors = new Array(); // An array containing all defined selectors.
Drupal.Sweaver.css = new Object(); // Object with all targets and their properties.
Drupal.Sweaver.path = new Array(); // Full path to the root of the document.
Drupal.Sweaver.pathIndexes = new Array(); // An array with the indexes of all selected items.
Drupal.Sweaver.activePath = ''; // Currently active path.
Drupal.Sweaver.activeElement = new Object(); // Currently active element.
Drupal.Sweaver.updateMode = true; // should the form updates be saved in css?

/**
 * Hook onload behavior
 */
$(document).ready(function() {

  Drupal.Sweaver.init();

  Drupal.Sweaver.writeCss();

  Drupal.Sweaver.addSliders();

  Drupal.Sweaver.addColorPicker();

  Drupal.Sweaver.updateForm();

  Drupal.Sweaver.bindClicks();

});

/**
 * Implementation of HOOK_updateCss().
 *
 * Return editor css.
 */
function sweaver_plugin_editor_updateCss() {
  var css = '';
  var fullCss = '';
  var cssContent = '';

  for (var key in Drupal.Sweaver.css) {
    var target = Drupal.Sweaver.css[key];
    for (var prop in target) {
      if (Drupal.Sweaver.properties[prop]) {
	    	// Special case for transparent.
	    	if (prop == 'background-color' && target[prop] == 'transparent') {
	          cssContent += '  '+ prop + ': ' + target[prop] + ';\n';
	    	}
        // Special case for background image.
        else if (prop == 'background-image' && target[prop] == '') {
            cssContent = '';
        }        
	    	else {
	          cssContent += '  '+ prop + ': ' + Drupal.Sweaver.properties[prop].prefix + target[prop] + Drupal.Sweaver.properties[prop].suffix + ';\n';
	    	}
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
  }

  // Store css in hidden field in save form
  $("#sweaver #edit-css").val($.toJSON(Drupal.Sweaver.css));

  // Add inline css
  $("#sweaver-form #edit-css").val(fullCss);

  return fullCss;
}

/**
 * Initialize member variables and properties.
 */
Drupal.Sweaver.init = function() {

  // Add sweaver class for extra margin at bottom.
  $('body').addClass('sweaver');

  // Get previously stored information or create empty object with all targets
  db_css = $("#edit-css");

  if (db_css.val() && db_css.val() != '[]'){
    Drupal.Sweaver.css = $.evalJSON(db_css.val());
    db_css.val('');
  }

  // Sweaver selectors.
  Drupal.Sweaver.selectors = Drupal.settings.sweaver['selectors'];
  
  // A string containing all selectors, used to avoid looping later on in the code.
  $.each(Drupal.Sweaver.selectors, function (index, selector) {
    Drupal.Sweaver.selectorsSelectors.push(selector.selector);
  });

  // Sweaver types.
  Drupal.Sweaver.types = Drupal.settings.sweaver['types'];

  // Sweaver properties.
  Drupal.Sweaver.properties = Drupal.settings.sweaver['properties'];

  // Classes that will never be used in the paths or generated css.
  Drupal.Sweaver.excludeClasses = Drupal.settings.sweaver['exclude_classes'];
  
  // Add div to store inline css and fill it up
  $('body > div:first').before('<div id="sweaver-css"></div>');

  // Add a link to be able to follow links.
  $('body').append('<a href="" id="follow-link">' + Drupal.t('Click here to follow this link') + '</div>');
}

/**
 * Get all values and update the form
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
	        var value = $(Drupal.Sweaver.activePath).css(object);
	        if(!isEmpty(Drupal.Sweaver.properties[object]) && Drupal.Sweaver.properties[object].type == 'color') {
	          $('#' + object + ' .colorSelector div').css('backgroundColor', value);
	        }
	        else {
	          if (value) {
	            $("#sweaver_plugin_editor #edit-" + object).val(value.replace('px', ''));
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

  Drupal.Sweaver.hideOverlays();

  // First hide titles and groups
  $('#sweaver #sweaver_plugin_editor h2, #sweaver #sweaver_plugin_editor .sweaver-group').each(function (i) {
    $(this).hide();
  });

  if (!isEmpty(Drupal.Sweaver.activeElement)) {
    // Decide which items should be shown or hidden. 	  
    var type = Drupal.Sweaver.activeElement.type;
	  $.each(Drupal.Sweaver.properties, function(index, object){
	    if(object.name in Drupal.Sweaver.types[type]) {
	      $('#sweaver #edit-' + object.name + '-wrapper').show();
        $('#sweaver #edit-' + object.name + '-wrapper').parents('.sweaver-group').show();	   
        $('#sweaver #edit-' + object.name + '-wrapper').parents('.container-inner').children('h2').show();           
	    } else {
	      $('#sweaver #edit-' + object.name + '-wrapper').hide();    
	    }
	  });
  } 
  else {
    $.each(Drupal.Sweaver.properties, function(index, object){
      $('#sweaver #edit-' + object.name + '-wrapper').hide(); 
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
    	  var current_color_value = ($('div', this).css('background-color')).replace('rgb(', '').replace(')', '').split(',');
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
          Drupal.Sweaver.hideChanges();
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

  // Never select the editor form as themeable.
  $(excludes).click(function(event) {
    event.stopPropagation();
  });

  // Catch click on a selector.
  $.each(Drupal.Sweaver.selectors, function (index, object) {
    var selectors = $(object.selector).filter(':parents(' + excludes + '):not(' + excludes + ')');
    selectors.click(function(event) {
      event.stopPropagation();
      // Only do something when the content area is visible.
      if (Drupal.Sweaver.open == 'true' && $('#sweaver_plugin_editor .sweaver-content').is(':visible')) {
        $('#sweaver_plugin_editor .sweaver-header').html('<div id="full-path" class="clear-block"></div><div id="selected-path" class="clear-block"></div>');
        
        var element = $(this);
        
	      // handle clicking on a link.
	      $('#follow-link').hide();
	      if(object.selector == 'a' && element.attr('id') != 'follow-link') {
	        var position = element.offset();
	        $('#follow-link').attr('href', element.attr('href')).css({'top' : position.top + element.outerHeight() + 5, 'left': position.left}).fadeIn();
	        event.preventDefault();
	      }
	      
	      if (element.attr('id') != 'follow-link') {
	        // Reset some values.
	        Drupal.Sweaver.path.length = 0;
	        Drupal.Sweaver.pathIndexes.length = 0;
	        $("#selected-path").html('');
	        $("#full-path").html('');

	        // Build path with parents.
	        Drupal.Sweaver.buildPath(this);

	        Drupal.Sweaver.updateForm();
	      }
      }
    });
  });

	// Toggle changes area.
	$('#changes-toggler').click(function(event){
	  event.stopPropagation();
	  toggler = $('#editor-changes');
	  if (parseInt(toggler.css('left')) < 0) {
		  Drupal.Sweaver.writeChanges();
		  toggler.css({'left' : '0px'});
      $(this).toggleClass('open').html(Drupal.t('Hide changes'));
		} else {
      toggler.css({'left' : '-9000px'});
      $(this).toggleClass('open').html(Drupal.t('Show changes'));
		}
	});

  // Hide sliders and close groups when clicking outside of them.
  $("#sweaver").click(function() {
    Drupal.Sweaver.hideOverlays();
    Drupal.Sweaver.hideChanges();
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

	    var left = slider.siblings('label').width() - 10;
	    var top = slider.siblings('.slider-value').outerHeight() + parseInt(slider.parent().css('padding-top')) + 5;
	    slider.css({'left' : left, 'top' : top}).css({'visibility' : 'visible'});
	  }
  });
}

/**
 * Store the parents of a clicked item.
 */
Drupal.Sweaver.buildPath = function(object) {
  var index = 0;

  // Collect info on currently active item.
  Drupal.Sweaver.activeElement.id = $(object).attr('id');
  Drupal.Sweaver.activeElement.class = $(object).attr('class').split(' ');
  Drupal.Sweaver.activeElement.tag = $(object).get(0).tagName.toLowerCase();
  Drupal.Sweaver.activeElement.type = $(object).css('display');
  Drupal.Sweaver.activeElement.object = object;

  // Add active element to first element in the path array.
  Drupal.Sweaver.path[0] = new Object({'id' : Drupal.Sweaver.activeElement.id, 'class' : Drupal.Sweaver.activeElement.class, 'tag' : Drupal.Sweaver.activeElement.tag,  'type' : Drupal.Sweaver.activeElement.type, 'object' : Drupal.Sweaver.activeElement.object});

  // Show the currenty active path and the full path.
  Drupal.Sweaver.addToFullPath(object, index, true);
  Drupal.Sweaver.addToActivePath(0, object);

  // Traverse all parents and save them in the path array.
  var i = 1;
  var active;
  $(object).parents().each(function() {
    active = false;

	  id = $(this).attr('id');
	  class = $(this).attr('class').split(' ');
	  tag = $(this).get(0).tagName.toLowerCase();
    type = $(this).css('display');
	  item = this;

    if (Drupal.Sweaver.selectorsSelectors.find('#' + id) || Drupal.Sweaver.selectorsSelectors.find('.' + class[0]) || Drupal.Sweaver.selectorsSelectors.find(tag)) {
      Drupal.Sweaver.path[i] = new Object({'id' : id, 'class' : class, 'tag' : tag, 'type' : type, 'object' : item});

	    // If selector is tagged as 'highlight', automatically select it.
	    $.each(Drupal.Sweaver.selectors, function (index, selector) {
	      if (selector.highlight == '1' && (selector.selector == '#' + id || selector.selector == '.' + class || selector.selector == tag)) {
	        active = true;
	        Drupal.Sweaver.addToActivePath(i, item);
	      }
	    });
	
	    // Add all items to the full path except for the html tag.
	    if (tag != 'html') {
	      Drupal.Sweaver.addToFullPath(item, i, active);
	    }
	    i++;
    }
  });
  Drupal.Sweaver.printActivePath();
  $("#full-path").prepend('<span class="label">' + Drupal.t('Full path: ') + '</span>');
}

/**
 * Print the full path.
 */
Drupal.Sweaver.addToFullPath = function(object, index, active) {
  var path_separator = ' > ';
  var active_class = '';

  // Don't show a seperator after the last item in the path.
  if (index == 0) {
    path_separator = '';
  }

  // Add an active class to the selected items.
  if (active == true) {
    active_class = ' class="active"';
  }

  // Populate path with clickable links.
  $("#full-path").prepend($('<span id="thid-' + index + '" '+ active_class +'><a href="#">' + Drupal.Sweaver.objectToReadable(Drupal.Sweaver.path[index]) + '</a> '+ path_separator +' </span>').click(function() {
    $(this).toggleClass('active');
    Drupal.Sweaver.addToActivePath(index, $(object));
    Drupal.Sweaver.printActivePath();
	  // Reset the active element as it might have changed.
	  Drupal.Sweaver.pathIndexes.sort(function(a,b){return a - b});
	  Drupal.Sweaver.activeElement = Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[0]] ? Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[0]] : {} ;
	  Drupal.Sweaver.updateForm();

    // Stop the link from doing anything.
    return false;
  }));
}

/**
 * Add an item to the active path.
 */
Drupal.Sweaver.addToActivePath = function(i, item) {
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
  for ( var i=0, len=Drupal.Sweaver.pathIndexes.length; i<len; ++i ){
    if (i > 0) {
      $("#selected-path").append(' in ');
    }
    $("#selected-path").append(Drupal.Sweaver.objectToReadable(Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]]));
  }
  $("#selected-path").prepend('<span class="label">' + Drupal.t('Selected item: ') + '</span>');

  // Save the currently active css path.
  Drupal.Sweaver.activePath = '';
  Drupal.Sweaver.pathIndexes.reverse();
  for (var i=0, len=Drupal.Sweaver.pathIndexes.length; i<len; ++i){
    Drupal.Sweaver.activePath += Drupal.Sweaver.objectToCss(Drupal.Sweaver.path[Drupal.Sweaver.pathIndexes[i]]) + ' ';
  }
}

/**
 * Fill the activeElement and update the Form and ActivePath.
 * Other plugins can use this function to set values on the
 * style tab. To switch tabs, you can use the Drupal.Sweaver.switchTab
 * function.
 */
Drupal.Sweaver.updateStyleTab = function(class_name, class_object) {

  Drupal.Sweaver.activeElement.id = '';
  Drupal.Sweaver.activeElement.class = new Array(class_name);
  Drupal.Sweaver.activeElement.tag = 'div';
  Drupal.Sweaver.activeElement.type = 'block';
  Drupal.Sweaver.activeElement.object = class_object;

  $('#sweaver_plugin_editor .sweaver-header').html('<div id="selected-path" class="clear-block"></div>');
  Drupal.Sweaver.path[0] = new Object({'id' : Drupal.Sweaver.activeElement.id, 'class' : Drupal.Sweaver.activeElement.class, 'tag' : Drupal.Sweaver.activeElement.tag,  'type' : Drupal.Sweaver.activeElement.type, 'object' : Drupal.Sweaver.activeElement.object});
  Drupal.Sweaver.addToFullPath(class_object, 0, true);
  Drupal.Sweaver.activePath = '';
  Drupal.Sweaver.pathIndexes = new Array();  
  Drupal.Sweaver.addToActivePath(0, class_object);
  Drupal.Sweaver.printActivePath();
  Drupal.Sweaver.updateForm();
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
}

/**
 * Write changes.
 */
Drupal.Sweaver.writeChanges = function() {
  $('#editor-changes').html('');
  for (key in Drupal.Sweaver.css) {
    var target = Drupal.Sweaver.css[key];
	for (prop in target) {
      if (Drupal.Sweaver.properties[prop]) {
    	// Special case for transparent.
    	if (prop == 'background-color' && target[prop] == 'transparent') {
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

  var translation = '';

  // First handle selectors defined in the backend.
  $.each(Drupal.Sweaver.selectors, function() {
    if (this.selector == '#' + object.id || this.selector == '.' + object.class[0] || this.selector == object.tag) {  
      translation = this.description;
      return false;
    }
  });
  
  // Else build a generic translation.
  if (translation == '') {
	  if (object.tag == 'body') {
	    translation = Drupal.Sweaver.selectors.body.description;
	  } 
	  else {
		  if (object.id) {
	      translation = 'the ' + object.id + ' region';
		  }
		  else if (object.class[0] && !Drupal.Sweaver.excludeClasses.find(object.class[0])) {
	      translation = 'all ' + object.class[0];
		  }
		  else if (object.tag) {
		    translation = object.tag;
		  }
	  }
	}
  return translation;
}

/**
 * Translate an parent item in a css name.
 */
Drupal.Sweaver.objectToCss = function(object) {
  
  var css = '';
  
  // First handle selectors defined in the backend.
  $.each(Drupal.Sweaver.selectors, function() {
    if (this.selector == '#' + object.id || this.selector == '.' + object.class[0] || this.selector == object.tag) {
      css = this.selector;
      return false;
    }
  });
  
  // Else build a generic translation.  
  if (css == '') {  
	  if (object.id) {
	    css = '#' + object.id;
	  }
	  else if (object.class[0] && !Drupal.Sweaver.excludeClasses.find(object.class[0])) {
	    css = '.' + object.class[0];
	  }
	  else if (object.tag) {
	    css = object.tag;
	  }
  }
  return css;
}

Drupal.Sweaver.hideOverlays = function() {
  // Hide all sliders.
  $('#sweaver .slider-wrapper').css({'visibility' : 'hidden'});

  // Hide all groups.
  $('#sweaver .sweaver-group-active .sweaver-group-content').css({'visibility' : 'hidden', 'right' :  '-10000px'});
  $('#sweaver .sweaver-group-active').removeClass('sweaver-group-active');
}

Drupal.Sweaver.hideChanges = function() {
  $('#editor-changes').css({'left' : '-9000px'});
  $('#changes-toggler').removeClass('open').html(Drupal.t('Show changes'));
}

Drupal.behaviors.openGroup = function() {

  // Open a group when it is clicked.
  $('#sweaver .sweaver-group').click(function (event) {
    event.stopPropagation();
    Drupal.Sweaver.hideOverlays();
    Drupal.Sweaver.hideChanges();

    var content = $('.sweaver-group-content', this);
    if(!content.is(':visible')) {

      // Open the clicked group and make sure it does not cover the label or fall off the screen.
      group = $(this);
      var right = 0;
      var top = group.outerHeight() - 10;
      var position = group.offset();
      if (position.top + content.outerHeight() > $(window).height()) {
        top = - content.height();
      }
      content.css({'right' : right, 'top' : top, 'visibility' : 'visible'}).parent().toggleClass('sweaver-group-active');
    }
  });
}


/**
 * Add custom expression to exclude all selectors in the sweaver bar.
 */
$.expr[':'].parents = function(a,i,m){
  return jQuery(a).parents(m[3]).length < 1;
};

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
 * Helper function to check if a string is in an array.
 */
Array.prototype.find = function (element) {
	for (var keys in this) {
	  if (this[keys] == element) {
	    return true;
	    break;
	  }  
	}
	return false;
};