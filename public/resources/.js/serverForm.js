'use strict';

function submitForm() {
  var $alertBox = $('#alertBox');
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for server creation.')).slideDown();

  var submitThis = true;
  var inputs = [
    'serverTitle',
    'serverURL',
  ];

  var data = {};
  inputs.forEach(function(value) {
    switch (checkFormElement('#' + value)) {
      case true:
        data[value] = getInputValue('#' + value);
        break;

      case false:
        submitThis = false;
        break;
      default:
        // Skip value
        break;
    }
  });

  if (submitThis) {
    $.ajax({
      type: "POST",
      url: '/ajax/servers/server/processNewServer',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response['status']) != 'undefined') {
          if (typeof(response['serverID']) != 'undefined') {
            if (response['serverID']) {
              window.location.href = '/servers/' + response['serverID'];
            } else {
              // Tell user error
              $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
          }
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
      })
      .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
      });
  } else {
    $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
  }
}

function updateForm() {
  var $alertBox = $('#alertBox');
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for server update.')).slideDown();

  var submitThis = false;
  var inputs = [
      'serverTitle',
      'serverIP',
  ];

  var data = {};
  inputs.forEach(function(value) {
    switch (checkFormElement('#' + value)) {
      case true:
        data[value] = getInputValue('#' + value);
        submitThis = true;
        break;
      default:
        // Skip value
        break;
    }
  });

  if (submitThis) {
    // Append server ID
    data['serverID'] = $('#serverID').val();
      
    $.ajax({
      type: "POST",
      url: '/ajax/servers/server/processUpdateServer',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $alertBox.html(bootstrapAlert('success', 'Server successfully edited.')).slideDown();
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
      })
      .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
      });
  } else {
    $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
  }
}

function deleteServer() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for server deletion.')).slideDown();
    var data = {'serverStatus': 0};

    // Append server ID
    data['serverID'] = $('#serverID').val();

    $.ajax({
        type: "POST",
        url: '/ajax/servers/server/processUpdateServer',
        data: {
            data: data
        }
    })
    .done(function(response) {
        if (typeof(response.status) != 'undefined') {
            $alertBox.html(bootstrapAlert('success', 'Server successfully deleted.')).slideDown();
        }
    })
    .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
    });
}
