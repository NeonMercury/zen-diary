var notes = {
	current_note : 0,

	clearNotes : function()
	{
		$("#notes-list").html("");
	},

	addNote : function(note)
	{
		const desc_max_length = 40;

		var li = $("<li>").addClass("list-group-item note-item").attr("note-id", note.id);
		var a = $("<a>").attr("href", "javascript:void(0)").attr("note-id", note.id).addClass("clear text-ellipsis on-btn-show-note");
		var updated = $("<small>").addClass("pull-right").html(note.updated);
		var title = $("<strong>").addClass("block").html(note.title);
		var desc = $("<small>");

		if (!note.encrypted)
		{
			var note_text = note.note;
			if (note_text.length > desc_max_length)
			{
				note_text = note_text.substr(0, desc_max_length);
				note_text = note_text + "&#8230;";
			}
			$(desc).html(note_text);

			$(li).prop("encrypted", false);
			$(a).prop("encrypted", false);
		}
		else
		{
			$(desc).html("<em>(Заметка зашифрована)</em>");

			$(li).prop("encrypted", true);
			$(a).prop("encrypted", true);
		}

		if (note.hidden)
		{
			$(li).addClass("hidden-note");
			$(li).prop("hidden", true);
		}

		$(a).append(updated).append(title).append(desc);
		$(li).append(a);

		$("#notes-list").append(li);
	},

	refreshNotes : function()
	{
		notes.clearNotes();

		var zen_notes = zen.getNotes();

		for (var i = 0; i < zen_notes.length; i++)
		{
			var note = zen_notes[i];

			notes.addNote(note);
		}
	},

	setNote : function(note)
	{
		notes.current_note = note.id;

		var html_text = markdown.toHTML(note.note);

		$("#note-title").html(note.title);
		$("#note-text").html(html_text);

		$("#view-note").fadeIn(zenapi.animation_duration);

		if (note.hidden)
		{
			$(".on-btn-hide").hide();
			$(".on-btn-show").show();
		}
		else
		{
			$(".on-btn-hide").show();
			$(".on-btn-show").hide();
		}
	},

	clearNote : function()
	{
		notes.current_note = 0;
		$("#note-title").html("");
		$("#note-text").html("");
		$("#view-note").hide();
	}
};

$(document).ready(function()
{
	notes.refreshNotes();
});

$(document).on("keydown", "#input-note-password", function(e)
{
	if (e.keyCode == 13)
	{
		e.preventDefault();
		$("#on-btn-enter-password").click();
	}
});

$(document).on("click", ".on-btn-show-note", function(e)
{
	e.preventDefault();
	var id = $(this).attr("note-id");
	var encrypted = $(this).prop("encrypted");	

	if (encrypted)
	{
		var dialog_message = zen.getTemplate("../media/httpdocs/templates/input-password.html");

		var dialog = new BootstrapDialog({
			title : "Заметка зашифрована",
			message : dialog_message,
			type : BootstrapDialog.TYPE_DEFAULT,
			buttons : [
				{
					id : "on-btn-enter-password",
					label : "OK"
				}
			],

			onshown : function(dlg)
			{
				$("#input-note-password").focus();
			}		
		});

		dialog.realize();

		var btn_enter_pasword = dialog.getButton("on-btn-enter-password");
		btn_enter_pasword.click({}, function(e)
		{
			var password = $("#input-note-password").val();
			var result = zen.getNote(id, password);			
			if (result.success)
			{
				notes.setNote(result);
				$.notify("Заметка расшифрована успешно.", 
				{
					position: "top right",
					className : "success"
				});
			}
			else
			{
				$.notify(result.message, 
				{
					position: "top right"
				});
			}

			dialog.close();
		});

		dialog.open();
	}
	else
	{
		var result = zen.getNote(id);			
		if (result.success)
		{
			notes.setNote(result);
		}
		else
		{
			$.notify(result.message, 
			{
				position: "top right"
			});
		}
	}
});

$(".on-btn-only-plain").click(function(e)
{
	e.preventDefault();
	var title = $(this).html();

	$(".filter-label").html(title);

	$(".note-item").each(function()
	{
		var encrypted = $(this).prop("encrypted");
		var hidden = $(this).prop("hidden");
		
		if (encrypted || hidden)
		{
			$(this).slideUp(zenapi.animation_duration);
		}
		else
		{
			$(this).slideDown(zenapi.animation_duration);
		}
	});
});

$(".on-btn-only-encrypted").click(function(e)
{
	e.preventDefault();
	var title = $(this).html();

	$(".filter-label").html(title);

	$(".note-item").each(function()
	{
		var encrypted = $(this).prop("encrypted");
		var hidden = $(this).prop("hidden");
		
		if (encrypted && !hidden)
		{
			$(this).slideDown(zenapi.animation_duration);
		}
		else
		{
			$(this).slideUp(zenapi.animation_duration);
		}
	});
});

$(".on-btn-filter-all").click(function(e)
{
	e.preventDefault();
	var title = $(this).html();

	$(".filter-label").html(title);

	$(".note-item").each(function()
	{			
		var hidden = $(this).prop("hidden");
		if (hidden)
		{
			$(this).slideUp(zenapi.animation_duration);
		}
		else
		{
			$(this).slideDown(zenapi.animation_duration);
		}
	});
});

$(".on-btn-only-hidden").click(function(e)
{
	e.preventDefault();
	var title = $(this).html();

	$(".filter-label").html(title);

	$(".note-item").each(function()
	{			
		var hidden = $(this).prop("hidden");
		if (!hidden)
		{
			$(this).slideUp(zenapi.animation_duration);
		}
		else
		{
			$(this).slideDown(zenapi.animation_duration);
		}
	});
});

$(".on-btn-edit").click(function(e)
{
	e.preventDefault();
	if (notes.current_note != 0)
	{
		window.location.href = "index.html?" + notes.current_note;
	}
});

$(".on-btn-hide").click(function(e)
{
	e.preventDefault();
	if (notes.current_note != 0)
	{
		var result = zen.hideNote(notes.current_note);
		if (result.success)
		{
			var note_id = notes.current_note;

			notes.clearNote();

			$(".note-item").each(function()
			{
				var id = $(this).attr("note-id");
				if (id == note_id)
				{
					var real_this = this;
					$(this).prop("hidden", true);
					$(this).slideUp(zenapi.animation_duration, function()
					{
						$(real_this).remove();
					});				
				}
			});

			notes.current_note = 0;

			$.notify("Заметка скрыта из дневника.", 
			{
				position: "top right",
				className : "success"
			});
		}
		else
		{
			$.notify(result.message, 
			{
				position: "top right"
			});
		}
	}
});

$(".on-btn-show").click(function(e)
{
	e.preventDefault();
	if (notes.current_note != 0)
	{
		var result = zen.showNote(notes.current_note);
		if (result.success)
		{		
			var note_id = notes.current_note;

			notes.clearNote();

			$(".note-item").each(function()
			{
				var id = $(this).attr("note-id");
				if (id == note_id)
				{
					var real_this = this;
					$(this).prop("hidden", false);
					$(this).slideUp(zenapi.animation_duration, function()
					{
						$(real_this).remove();
					});				
				}
			});

			notes.current_note = 0;

			$.notify("Заметка вновь показывается в дневнике.", 
			{
				position: "top right",
				className : "success"
			});
		}
		else
		{
			$.notify(result.message, 
			{
				position: "top right"
			});
		}
	}
});

$(".on-btn-delete").click(function(e)
{
	e.preventDefault();
	if (notes.current_note != 0)
	{
		var result = zen.deleteNote(notes.current_note);
		if (result.success)
		{		
			var note_id = notes.current_note;

			notes.clearNote();

			$(".note-item").each(function()
			{
				var id = $(this).attr("note-id");
				if (id == note_id)
				{
					var real_this = this;
					$(this).slideUp(zenapi.animation_duration, function()
					{
						$(real_this).remove();
					});				
				}
			});

			notes.current_note = 0;

			$.notify("Сообщение удалено из дневника навсегда.", 
			{
				position: "top right",
				className : "success"
			});
		}
		else
		{
			$.notify(result.message, 
			{
				position: "top right"
			});
		}
	}
});