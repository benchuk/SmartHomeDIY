(function() {

//$('#light1').on('click', function(){
//    $.ajax({
//        type: "GET",
//        url: '/light1',
//        success: function(data){
//           console.log(data);
//    });
//});
//$('#light1').on('click', function(){
//	$.ajax({
//	  url: "/light1",
//	  context: document.body
//	}).done(function() {
//		console.log(data);
//	});
//});

var btnClassClick = function(e){
    //alert("Button clicked from class: "+e.currentTarget.id);
	$.ajax({
	  url: "/harq/?value=" + e.currentTarget.id,
	  context: document.body
	}).done(function() {
		console.log("done");
	});

}

$('.btn').on('click', btnClassClick);



})(jQuery);
