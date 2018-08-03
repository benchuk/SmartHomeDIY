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
		console.log(e.currentTarget.id);
	});

}

$('.btn').on('click', btnClassClick);

$('.showrecord').click(function() {
	$('#recordForm').css({"display":"inline-block"});
});


$('#close').click(function() {
	$('#recordForm').css({"display":"none"});
});



})(jQuery);
