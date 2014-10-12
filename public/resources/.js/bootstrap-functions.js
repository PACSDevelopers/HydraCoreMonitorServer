'use strict';

/* Custom bootstrap functions */
function bootstrapAlert(status,message){
    function capitaliseFirstLetter(string)
    {
        return string.charAt(0).toUpperCase() + string.slice(1);
    }
    return '<div class="alert alert-'+ status +' alert-dismissable"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button><strong>' + capitaliseFirstLetter(status) +': </strong>' + message +'</div>';
}