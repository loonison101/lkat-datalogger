using Microsoft.EntityFrameworkCore.Migrations;

namespace LKAT.Cmd.Migrations
{
    public partial class AddedBatteryVoltage : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<int>(
                name: "BatteryVoltge",
                table: "CsvRecords",
                nullable: false,
                defaultValue: 0);
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "BatteryVoltge",
                table: "CsvRecords");
        }
    }
}
