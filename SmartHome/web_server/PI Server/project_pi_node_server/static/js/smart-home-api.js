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

var toogle
var btnClassClick = function(e){
	//alert("Button clicked from class: "+e.currentTarget.id);
	$.ajax({
	  url: "/harq/?value=" + e.currentTarget.id,
	  context: document.body
	}).done(function() {
		console.log(e.currentTarget.id);
	});
	$(this).removeClass("btn-primary");	
	$(this).addClass("btn-info");	
	setTimeout(() => {
		$(this).removeClass("btn-info");	
		$(this).addClass("btn-primary");	
	}, 700);
}

$('.btn').on('click', btnClassClick);

$('.showrecord').click(function() {
	$('#recordForm').css({"display":"inline-block"});
});


$('#close').click(function() {
	$('#recordForm').css({"display":"none"});
});

$('.deletecmd').click(function(e) {
	$.ajax({
		url: "/deletecmd/?value=" + e.currentTarget.id,
		context: document.body
	  }).done(function() {
		  console.log("delete done: " + e.currentTarget.id);
		  location.reload();
	  });
});



})(jQuery);
