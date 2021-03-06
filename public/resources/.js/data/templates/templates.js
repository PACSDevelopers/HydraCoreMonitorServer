'use strict';

function createTemplate() {
  var $alertBox = $('#alertBox');
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for template creation.')).slideDown();

  var submitThis = true;
  var inputs = [
    'templateTitle',
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
      url: '/ajax/templates/template/processNewTemplate',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response['status']) != 'undefined') {
          if (typeof(response['templateID']) != 'undefined') {
            if (response['templateID']) {
              window.location.href = '/data/templates/' + response['templateID'];
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

function updateTemplate() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for template update.')).slideDown();

    var submitThis = false;
    var inputs = [
        'templateTitle',
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
        // Append template ID
        data['templateID'] = $('#templateID').val();

        $.ajax({
            type: "POST",
            url: '/ajax/templates/template/processUpdateTemplate',
            data: {
                data: data
            }
        })
            .done(function(response) {
                if (response.status) {
                    $alertBox.html(bootstrapAlert('success', 'Template successfully edited.')).slideDown();
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

function deleteTemplate() {
    var confirmation = confirm('Are you sure you want to remove this template?');
    if(!confirmation) {
        return;
    }

    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for template deletion.')).slideDown();
    var data = {'templateStatus': 0};

    // Append template ID
    data['templateID'] = $('#templateID').val();

    $.ajax({
        type: "POST",
        url: '/ajax/templates/template/processUpdateTemplate',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                $alertBox.html(bootstrapAlert('success', 'Template successfully deleted.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function loadTables() {
    var $alertBox = $('#alertBox');
    var data = {'templateID': $('#templateID').val()};

    $alertBox.html(bootstrapAlert('info', 'Downloading template.')).slideDown();
    
    $.ajax({
        type: "POST",
        url: '/ajax/templates/table/loadAll',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                $alertBox.html(bootstrapAlert('info', 'Loading template.'));
                
                setTimeout(function(){
                    var $tableList = $('#tableList');
                    bindEvents();

                    window.templateData = response.result;

                    window.tableColumns = {0: []};

                    window.templateData.forEach(function(value, index){
                        window.tableColumns[value['id']] = value['columns'];
                        window.templateData[index]['columns'] = undefined;
                    });
                    
                    window.templateData.forEach(function(value){
                        generateTableHTML(value['id'], value['name'], value['alias'], $tableList);
                    });

                    populateRelatedTables();
                    $alertBox.slideUp();
                }, 1000);
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function bindEvents() {
    $(document).on('change', '.tableColumnRelatedTable', function(){
        var $this = $(this);
        var id = $this.val();
        var orgValue = $this.attr('data-orgvalue');
        if(id != orgValue) {
            $this.attr('data-orgvalue', id);
            updateColumn({'tableColumnRelatedTable': id}, $this);
        }
        
        var column = $this.parent().parent().parent();
        var columnID = column.attr('data-id');
        
        var table = column.parent().parent();
        var tableID = table.attr('data-id');
        
        if(id == 0) {
            var columnsHtml = '<option value="0" selected="selected">None</option>';
        } else {
            var columnsHtml = '<option value="0">None</option>';
        }
                
        window.tableColumns[id].forEach(function(value){
            columnsHtml += '<option class="tableColumn' + value['id'] + 'Option" value="' + value['id'] + '"></option>';
        });
        
        $('#table' + tableID + 'Column' + columnID + 'RelatedColumn').html(columnsHtml);

        window.tableColumns[id].forEach(function(value, index){
            $('#table' + tableID + 'Column' + columnID + 'RelatedColumn').children().eq(index + 1).text(value['name']);
        });
        
        var orgValue = $('#table' + tableID + 'Column' + columnID + 'RelatedColumn').attr('data-orgvalue');

        var selectedValue = 0;
        window.tableColumns[id].forEach(function(value){
            if(value['id'] == orgValue) {
                selectedValue = orgValue;
            }
        });
        
        $('#table' + tableID + 'Column' + columnID + 'RelatedColumn').val(selectedValue);
    });
    
    $(document).on('keyup', '.tableName', function(){
        var $this = $(this);
        var $table = $this.parent().parent().parent();
        var tableID = $table.attr('data-id');
        var tableName = $this.val();
        $('.table' + tableID + 'Option').text(tableName);

        window.templateData.forEach(function(value, index){
           if(value['id'] == tableID) {
               window.templateData[index]['name'] = tableName;
           } 
        });
    });
    
    $(document).on('keyup', '.tableColumnName', function(){
        var $this = $(this);
        var $column = $this.parent().parent().parent();
        var $table = $column.parent().parent();
        var columnID = $column.attr('data-id');
        var tableID = $table.attr('data-id');
        var columnName = $this.val();
        $('.tableColumn' + columnID + 'Option').text(columnName);

        window.tableColumns[tableID].forEach(function(value, index){
           if(value['id'] == columnID) {
               window.tableColumns[tableID][index]['name'] = columnName;
           }
        });
    });

    $(document).on('click', '.removeColumn', function(){
        removeColumn($(this));
    });

    $(document).on('click', '.removeTable', function(){
        removeTable($(this));
    });

    $(document).on('click', '.addColumn', function(){
        addColumn($(this));
    });

    $(document).on('click', '.addTable', function(){
        addTable();
    });


    $(document).on('change', '.tableName', function() {
        var $this = $(this);
        updateTable({'tableName': $this.val()}, $this);
    });

    $(document).on('change', '.tableAlias', function() {
        var $this = $(this);
        updateTable({'tableAlias': $this.val()}, $this);
    });

    $(document).on('change', '.tableColumnName', function() {
        var $this = $(this);
        updateColumn({'tableColumnName': $this.val()}, $this);
    });

    $(document).on('change', '.tableColumnAlias', function() {
        var $this = $(this);
        updateColumn({'tableColumnAlias': $this.val()}, $this);
    });

    $(document).on('change', '.tableColumnRelatedColumn', function() {
        var $this = $(this);
        updateColumn({'tableColumnRelatedColumn': $this.val()}, $this);
    });
}

function updateTable(data, element) {
    var $alertBox = $('#alertBox');
    var parent = element.parent().parent().parent();
    var id = parent.attr('data-id');
    data['tableID'] = id;

    $.ajax({
        type: "POST",
        url: '/ajax/templates/table/update',
        data: {
            data: data
        }
    }).fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function updateColumn(data, element) {
    var $alertBox = $('#alertBox');
    var parent = element.parent().parent().parent();
    var id = parent.attr('data-id');
    data['columnID'] = id;
    
    $.ajax({
        type: "POST",
        url: '/ajax/templates/column/update',
        data: {
            data: data
        }
    }).fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function addColumn(element) {
    var $alertBox = $('#alertBox');
    var parent = element.parent();
    var id = parent.attr('data-id');
    var data = {'tableID': id, 'templateID': $('#templateID').val()};

    $.ajax({
        type: "POST",
        url: '/ajax/templates/column/add',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                var html = '<div id="table' + id + 'Column' + response['id'] + '" data-id="' + response['id'] + '" class="form-group tableColumn"><div class="col-sm-11"><div class="col-sm-3"><input type="text" value="" id="table' + id + 'Column' + response['id'] + 'Name" class="form-control tableColumnName user-success"></div><div class="col-sm-3"><input type="text" value="" id="table' + id + 'Column' + response['id'] + 'Alias" class="form-control tableColumnAlias"></div><div class="col-sm-3"><select type="text" value="" id="table' + id + 'Column' + response['id'] + 'RelatedTable" data-orgvalue="0" class="form-control tableColumnRelatedTable user-success"><option value="0" selected="selected">None</option></select></div><div class="col-sm-3"><select type="text" id="table' + id + 'Column' + response['id'] + 'RelatedColumn" data-orgvalue="0" class="form-control tableColumnRelatedColumn"><option value="0" selected="selected">None</option></select></div></div><div class="col-sm-1"><button type="button" class="btn btn-default removeColumn pull-right" data-id="' + response['id'] + '"><span class="glyphicons remove"></span></button></div><div class="clearfix"></div><br /></div>';
                var columnsElement = element.parent().children('.tableColumns');
                columnsElement.append(html);

                window.tableColumns[id].push({'id': response['id'], 'name': '', 'alias': '', 'relationColumn': 0, 'relationTable': 0});

                populateRelatedTables();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function addTable() {
    var $alertBox = $('#alertBox');
    var element = $('#tableList');
    var data = {'templateID': $('#templateID').val()};

    $.ajax({
        type: "POST",
        url: '/ajax/templates/table/add',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                var html = '<div id="table' + response['id'] + '" data-id="' + response['id'] + '" class="row"><div class="form-group"><label class="col-sm-6 control-label">Table Name</label><label class="col-sm-6 control-label">Table Alias</label></div><div class="form-group"><div class="col-sm-6"><input type="text" value="" id="table' + response['id'] + 'Name" class="form-control tableName user-success"></div><div class="col-sm-6"><input type="text" value="" id="table' + response['id'] + 'Alias" class="form-control tableAlias"></div></div><div class="clearfix"></div><br><div id="table' + response['id'] + 'Columns" class="row tableColumns"><div class="form-group"><label class="col-sm-3 control-label">Column Name</label><label class="col-sm-3 control-label">Column Alias</label><label class="col-sm-3 control-label">Related Table</label><label class="col-sm-3 control-label">Related Column</label></div></div><button type="button" class="btn btn-default removeTable pull-right">Remove Table</button><button type="button" class="btn btn-default addColumn pull-right" style="margin-right: 10px;">Add Column</button><div class="clearfix"></div><br></div>';
                        
                window.tableColumns[response['id']] = [];

                window.templateData.push({'id': response['id'], 'name': '', 'alias': ''});

                element.append(html);

                populateRelatedTables();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function removeColumn(element) {
    var $alertBox = $('#alertBox');
    var id = element.attr('data-id');
    var parent = element.parent().parent().parent().parent();
    var tableID = parent.attr('data-id');
    var data = {'columnID': id};

    $.ajax({
        type: "POST",
        url: '/ajax/templates/column/delete',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                element.parent().parent().remove();
                window.tableColumns[tableID].forEach(function(value, index){
                    if(value['id'] == id) {
                        window.tableColumns[tableID].splice(index, 1);
                    }
                });
                $('.tableColumn' + id + 'Option').each(function(){
                   var $this = $(this);
                    if($this.is(':selected')) {
                        $this.parent().val(0).trigger('change');
                    }
                    $this.remove();
                });
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function removeTable(element) {
    var $alertBox = $('#alertBox');
    var parent = element.parent();
    var id = parent.attr('data-id');
    var data = {'tableID': id};

    $.ajax({
        type: "POST",
        url: '/ajax/templates/table/delete',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (response.status) {
                parent.remove();
                window.tableColumns[id] = undefined;
                window.templateData.forEach(function(value, index){
                    if(value['id'] == id) {
                        window.templateData.splice(index, 1);
                    }
                });
                $('.table' + id + 'Option').each(function(){
                    var $this = $(this);
                    if($this.is(':selected')) {
                        $this.parent().val(0).trigger('change');
                    }
                    $this.remove();
                });
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}


function generateTableHTML(id, name, alias, element) {    
    var html = '<div id="table' + id + '" data-id="' + id + '" class="row">';
    
        html += '<div class="form-group">';
            html += '<label class="col-sm-6 control-label">Table Name</label>';
            html += '<label class="col-sm-6 control-label">Table Alias</label>';
        html += '</div>';
    
        html += '<div class="form-group">';
    
            html += '<div class="col-sm-6">';
            
                html += '<input type="text" value="" id="table' + id + 'Name" class="form-control tableName">';
            
            html += '</div>';
        
            html += '<div class="col-sm-6">';
            
                html += '<input type="text" value="" id="table' + id + 'Alias" class="form-control tableAlias">';
            
            html += '</div>';
    
        html += '</div>';

        html += '<div class="clearfix"></div><br />';
    
        html += '<div id="table' + id + 'Columns" class="row tableColumns">';

        html += '<div class="form-group">';
            html += '<label class="col-sm-3 control-label">Column Name</label>';
            html += '<label class="col-sm-3 control-label">Column Alias</label>';
            html += '<label class="col-sm-3 control-label">Related Table</label>';
            html += '<label class="col-sm-3 control-label">Related Column</label>';
        html += '</div>'
        
            window.tableColumns[id].forEach(function(value){
                html += '<div id="table' + id + 'Column' + value['id'] + '" data-id="' + value['id'] + '" class="form-group tableColumn">';

                    html += '<div class="col-sm-11">';
                
                        html += '<div class="col-sm-3">';
        
                            html += '<input type="text" value="" id="table' + id + 'Column' + value['id'] + 'Name" class="form-control tableColumnName">';
        
                        html += '</div>';
    
                        html += '<div class="col-sm-3">';
    
                            html += '<input type="text" value="" id="table' + id + 'Column' + value['id'] + 'Alias" class="form-control tableColumnAlias">';
    
                        html += '</div>';
    
    
                        html += '<div class="col-sm-3">';
    
                            html += '<select type="text" value="" id="table' + id + 'Column' + value['id'] + 'RelatedTable" data-orgvalue="' + value['relationTable'] + '" class="form-control tableColumnRelatedTable"><option value="0" selected="selected">None</option></select>';
    
                        html += '</div>';
    
                        html += '<div class="col-sm-3">';
    
                            html += '<select type="text" id="table' + id + 'Column' + value['id'] + 'RelatedColumn" data-orgvalue="' + value['relationColumn'] + '" class="form-control tableColumnRelatedColumn"><option value="0" selected="selected">None</option></select>';
    
                        html += '</div>';

                    html += '</div>';

                    html += '<div class="col-sm-1">';
                        html += '<button type="button" class="btn btn-default removeColumn pull-right" data-id="' + value['id'] + '"><span class="glyphicons remove"></span></button>';
                    html += '</div>';

                html += '<div class="clearfix"></div><br />';
                
                html += '</div>';

                
            });
    
        html += '</div>';

    html += '<button type="button" class="btn btn-default removeTable pull-right">Remove Table</button>';
    html += '<button type="button" class="btn btn-default addColumn pull-right" style="margin-right: 10px;">Add Column</button>';
    
    html += '<div class="clearfix"></div><br />';
    html += '</div>';

    element.append(html);
    
    $('#table' + id + 'Name').val(name);
    $('#table' + id + 'Alias').val(alias);

    window.tableColumns[id].forEach(function(value){
        $('#table' + id + 'Column' + value['id'] + 'Name').val(value['name']);
        $('#table' + id + 'Column' + value['id'] + 'Alias').val(value['alias']);
    });
    
}


function populateRelatedTables() {
    
    var optionsHtml = '<option value="0" selected="selected">None</option>';
    window.templateData.forEach(function(value, index){
        optionsHtml += '<option class="table' + value['id'] + 'Option" value="' + value['id'] + '">' + value['name'] + '</option>';
    });
    
    $('.tableColumnRelatedTable').html(optionsHtml);

    $('.tableColumnRelatedTable').each(function() {
        var $this = $(this);
        var value = $this.attr('data-orgvalue');
        if(value != 0) {
            $this.val(value);
            $this.trigger('change');
        }
    });
}

function importTemplate(databaseID, schema) {
    var $alertBox = $('#alertBox');
    $('#templateModalSaveButton').attr('disabled', 'disabled');

    if(databaseID) {
        if(schema) {
            var data = {'templateID':  $('#templateID').val(), 'databaseID': databaseID, 'schema': schema};
            $.ajax({
                type: "POST",
                url: '/ajax/templates/template/processImportTemplate',
                data: {
                    data: data
                }
            }).done(function(response) {
                if (response.status) {
                    window.location.reload();
                }
            }).fail(function() {
                // Tell user error
                alert('Something went wrong, please try again');
                $('#templateModal').modal('hide');
            });
        } else {
            var data = {'templateID':  $('#templateID').val(), 'databaseID': databaseID};
            $.ajax({
                type: "POST",
                url: '/ajax/templates/template/processImportTemplate',
                data: {
                    data: data
                }
            }).done(function(response) {
                if (response.status) {
                    if(response.result) {
                        var select = '<select id="importTemplateModalSelect" class="form-control"><option selected="selected" disabled="disabled" value="">Please Select A Schema</option>';

                        response.result.forEach(function(value){
                            select += '<option>' + value + '</option>';
                        });

                        select += '</select>';

                        $('#templateModalBody').html(select);
                        $('#templateModalSaveButton').removeAttr('disabled');
                        $('#templateModalSaveButton').off('click');
                        $('#templateModalSaveButton').on('click', function(){
                            var $this = $('#importTemplateModalSelect');
                            if($this.val()) {
                                importTemplate(data['databaseID'], $this.val());
                            }
                        });
                        $('#templateModal').modal('show');
                    }
                }
            }).fail(function() {
                // Tell user error
                alert('Something went wrong, please try again');
                $('#templateModal').modal('hide');
            });
        }
    } else {
        var data = {'templateID':  $('#templateID').val()};

        $.ajax({
            type: "POST",
            url: '/ajax/templates/template/processImportTemplate',
            data: {
                data: data
            }
        }).done(function(response) {
            if (response.status) {
                if(response.result) {
                    var select = '<select id="importTemplateModalSelect" class="form-control"><option selected="selected" disabled="disabled" value="">Please Select A Database</option>';

                    response.result.forEach(function(value){
                        select += '<option value="' + value['id'] + '">' + value['title'] + '</option>';
                    });

                    select += '</select>';

                    $('#templateModalBody').html(select);
                    $('#templateModalSaveButton').removeAttr('disabled');
                    $('#templateModalSaveButton').off('click');
                    $('#templateModalSaveButton').on('click', function(){
                        var $this = $('#importTemplateModalSelect');
                        if($this.val()) {
                            importTemplate($this.val());
                        }
                    });
                    $('#templateModal').modal('show');
                }
            }
        }).fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
    }
}


$(document).ready(function(){
    if($('#templateID').val()) {
        loadTables();
    }
});