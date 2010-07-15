<?php
// $Id$

/**
 * @file
 * Themer editor form.
 */
?>
<div id="sweaver" <?php ($sweaver_editor_open == 'true') ? '' : print ' style="height:0"'; ?>>

  <!-- tabs -->
  <div id="sweaver-tabs" class="clear-block">
    <div class="close<?php ($sweaver_editor_open == 'true') ? '' : print ' active-tab'; ?>"><?php print '<a href="javascript:;">x</a>'; ?></div>
    <?php
    foreach ($tabs as $key => $tab):
    ?>
      <div id="tab-<?php print $key; ?>" class="tab <?php if ($active_tab == $key) print 'active-tab'; ?> <?php print $key; ?>">
        <a href="javascript:;"><?php print $tab['#tab_name']; ?></a>
      </div>
    <?php
    endforeach; ?>
  </div>

  <div id="sweaver-middle" class="clear-block">
     <?php
     foreach ($tabs_data as $key => $tab_data):
     ?>
     <!-- <?php print $key; ?> -->
      <div id="<?php print $key;?>">
       <div class="sweaver-header" <?php ($active_tab != $key) ? print 'style="display:none"' : '' ?>><?php print $tab_data['#tab_description']; ?></div>
       <div class="sweaver-content" <?php ($active_tab != $key) ? print 'style="display:none"' : '' ?>><?php print $tab_data['content']; ?></div>
      </div>
    <?php
      endforeach;
    ?>
  </div>

  <?php print $rest_of_form; ?>
</div>
