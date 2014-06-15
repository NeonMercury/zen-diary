$(document).ready(function()
{
	var login = zen.getUsername();

	$("#login").val(login);
	$("#db-path").val(zen.getDatabasePath());
	$("#on-check-use-scripts").prop("checked", zen.getUseJavascript());
	$("#on-check-show-line-numbers").prop("checked", zen.getShowLineNumbers());
	$("#preview-width").val(zen.getPreviewWidth());
});

$("#on-btn-save").click(function(e)
{
	e.preventDefault();

	var login = $("#login").val();
	var current_password = $("#current-password").val();
	var new_password = $("#new-password").val();
	var new_password_confirm = $("#new-password-confirm").val();

	var preview_width = $("#preview-width").val();

	var use_javascript = $("#on-check-use-scripts").is(":checked");
	var show_line_numbers = $("#on-check-show-line-numbers").is(":checked");

	var new_db_path = $("#db-path").val();	

	if (login.length == 0)
	{
		$.notify("Введите имя пользователя.",
		{
			position: "top right"
		});
		$("#login").focus();
		return;
	}

	if (new_db_path.length == 0)
	{
		$.notify("Укажите путь к базе данных.",
		{
			position: "top right"
		});
		$("#db-path").focus();
		return;
	}	

	if (new_password.length > 0)	
	{
		if (new_password != new_password_confirm)
		{
			$.notify("Введённые вами новые пароли не совпадают.",
			{
				position: "top right"
			});
			$("#new-password-confirm").focus();
			return;
		}

		var result = zen.loginUser(zen.getUsername(), current_password);
		if (!result.success)
		{
			$.notify("Вы неправильно ввели текущий пароль.",
			{
				position: "top right"
			});
			$("#current-password").focus();
			return;
		}

		result = zen.changeCredits(login, new_password);
		if (result.success)
		{
			$.notify("Пароль успешно изменён.",
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

	if (preview_width.length == 0)
	{
		$.notify("Вы не заполнили ширину блока предпросмотра.",
		{
			position: "top right"
		});
		$("#preview-width").focus();
		return;
	}

	preview_width = zen.toInt(preview_width);

	if (preview_width == undefined)
	{
		$.notify("Укажите числовое значение ширины блока предпросмотра.",
		{
			position: "top right"
		});
		$("#preview-width").focus();
		return;	
	}

	zen.setDatabasePath(new_db_path);
	zen.setUseJavascript(use_javascript);
	zen.setShowLineNumbers(show_line_numbers);
	zen.setPreviewWidth(preview_width);

	$.notify("Настройки успешно сохранены.",
	{
		position: "top right",
		className : "success"
	});
});

$("#on-btn-db-path").click(function(e)
{
	var result = zen.saveFileDialog("Файлы базы данных SQLite (*.sqlite3)", "*.sqlite3");

	const ext = ".sqlite3";

	if (result != undefined)
	{						
		if (result.length <= ext.length || zen.toLower(result.substr(result.length - ext.length)) != ext)
		{
			result = result + ext;
		}
		$("#db-path").val(result);
	}
});