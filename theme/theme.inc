<?php

/**
 * @file
 * Theme hooks.
 */

/**
 * Implements hook_preprocess().
 */
function islandora_video_preprocess_islandora_video(array &$variables) {
  module_load_include('inc', 'islandora', 'includes/datastream');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  module_load_include('inc', 'islandora', 'includes/authtokens');
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');
  $object = $variables['object'];
  $repository = $object->repository;
  // Get DC metadata.
  try {
    $dc = $object['DC']->content;
    $dc_object = DublinCore::importFromXMLString($dc);
  }
  catch (Exception $e) {
    drupal_set_message(t('Error retriveing object %s %t', array('%s' => $object->id, '%t' => $e->getMessage())), 'error', FALSE);
  }
  $variables['islandora_dublin_core'] = isset($dc_object) ? $dc_object : NULL;
  $variables['dc_array'] = isset($dc_object) ? $dc_object->asArray() : array();
  $variables['islandora_object_label'] = $object->label;
  $variables['theme_hook_suggestions'][] = 'islandora_video__' . str_replace(':', '_', $object->id);
  $variables['parent_collections'] = islandora_get_parents_from_rels_ext($object);
  $video_params = array();
  // Thumbnail.
  if (isset($object['TN'])) {
    $video_params = array(
      'tn_url' => url("islandora/object/{$object->id}/datastream/TN/view"),
    );
  }
  // Use OGG datastream for chrome but MP4 for everybody else.
  if (stristr($_SERVER['HTTP_USER_AGENT'], 'chrome') !== FALSE) {
    $viewer_dsid = 'OGG';
  }
  else {
    $viewer_dsid = 'MP4';
  }
  $viewer_dsid = 'MP4';
  $video_url = url("islandora/object/{$object->id}/datastream/$viewer_dsid/view");
  // Video player.
  $video_params = array(
    'url' => $video_url,
    'pid' => $object->id,
    'mime' => 'video/mp4',
    'tn' => url("islandora/object/{$object->id}/datastream/TN/view", array('absolute' => TRUE)),
  );
  $viewer = islandora_get_viewer($video_params, 'islandora_video_viewers', $object);
  $variables['islandora_content'] = '';
  if ($viewer) {
    $variables['islandora_content'] = $viewer;
  }
  else {
    $variables['islandora_content'] = NULL;
  }
  return array('' => $viewer);
}
