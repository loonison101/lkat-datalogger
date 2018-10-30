using System;
using Microsoft.EntityFrameworkCore.Migrations;

namespace LKAT.Cmd.Migrations
{
    public partial class InitialCreate : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "CsvRecords",
                columns: table => new
                {
                    Id = table.Column<int>(nullable: false)
                        .Annotation("Sqlite:Autoincrement", true),
                    Satellites = table.Column<int>(nullable: false),
                    Hdop = table.Column<float>(nullable: false),
                    Latitude = table.Column<float>(nullable: false),
                    Longitude = table.Column<float>(nullable: false),
                    Age = table.Column<float>(nullable: false),
                    When = table.Column<DateTime>(nullable: false),
                    Altitude = table.Column<float>(nullable: false),
                    Uuid = table.Column<string>(nullable: true),
                    Speed = table.Column<float>(nullable: false),
                    RawLine = table.Column<string>(nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_CsvRecords", x => x.Id);
                });
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "CsvRecords");
        }
    }
}
