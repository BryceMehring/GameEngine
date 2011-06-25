
void ClearButton(bool b)
{
	cls();
}


[final]
void main()
{

	CheckBoxData@ data = CheckBoxData();
	data.checked = false;
	data.name = "Clear";
	
	data.rect.left = 100;
	data.rect.top = 0;
	data.func = as.GetFunc(@ClearButton);

	ui.AddElement("CheckBox",@data);
}
