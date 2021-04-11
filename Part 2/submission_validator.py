# source logan/grading_tools
# Copyright 2020 Logan Gilmour
# Modified by Xinlei Chen
# Modified by Veronica Salm


import os
import zipfile
import tarfile
import shutil
from tempfile import gettempdir
from random import randint
from contextlib import contextmanager
import argparse


# CHANGE THIS FOR EACH WEEKLY EXERCISE
conf = {
    # the name of the assignment folder
    "submission_name": "part2",
    # the files that must be present
    "specified_files": ["README",
                        "Makefile",
                        "plotter",
                        "link_emulator",
                        "client/client.cpp",
                        "server/server.cpp",
                        "server/digraph.cpp",
                        "server/digraph.h",
                        "server/dijkstra.cpp",
                        "server/dijkstra.h",
                        "server/heap.h",
                        "server/wdigraph.h"],
    "assignment_version": "Assignment 1 Part 2 (Teams)",
}
course = "CMPUT 275"

class ValidationException(Exception):
    pass


def get_contents(path):
    names = []
    for root, subFolder, files in os.walk(path):
        # names = []
        for name in files:
            subdir = os.path.relpath(root, path)
            if subdir != ".":
                names.append(os.path.join(subdir, name))
            else:
                names.append(name)
    return sorted(names)


def validate_contents(archive_filename, extracted_dir, specified_files):
    found_files = get_contents(extracted_dir)

    if set(found_files) != set(specified_files):
    # if not set(specified_files).issubset(set(found_files)):
        raise ValidationException("{} should contain exactly:\n{}\n\nbut instead contains:\n{}"
                                      .format(archive_filename, "\n".join(sorted(specified_files)), "\n".join(found_files)))


def extract_submission(submission_name, extracted_dir):
    zip_submission = submission_name + ".zip"
    tar_submission_1 = submission_name + ".tar.gz"
    # For .tgz submissions
    """
    elif os.path.exists(tar_submission_2):
        archive_filename = tar_submission_2
        try:
            with tarfile.open(tar_submission_2, "r:gz") as tar_ref:
                tar_ref.extractall(extracted_dir)
        except Exception as e:
            raise ValidationException("An exception occurred while decompressing {}: {}".format(tar_submission_2, e))
    """# tar_submission_2 = submission_name + ".tgz"

    if os.path.exists(zip_submission):
        archive_filename = zip_submission
        try:
            with zipfile.ZipFile(zip_submission, 'r') as zip_ref:
                zip_ref.extractall(extracted_dir)
        except Exception as e:
            raise ValidationException("An exception occurred while decompressing {}: {}".format(zip_submission, e))

    elif os.path.exists(tar_submission_1):
        archive_filename = tar_submission_1
        try:
            with tarfile.open(tar_submission_1, "r:gz") as tar_ref:
                tar_ref.extractall(extracted_dir)
        except Exception as e:
            raise ValidationException("An exception occurred while decompressing {}: {}".format(tar_submission_1, e))
    else:
        raise ValidationException(
            "Couldn't find submission in the current directory. Your submission file should be called {} or {}"
                .format(zip_submission, tar_submission_1))

    return archive_filename, extracted_dir


@contextmanager
def extract_submission_temp(submission_name):
    extracted_dir = os.path.join(gettempdir(), str(randint(1000000, 9999999)))

    yield extract_submission(submission_name, extracted_dir)

    shutil.rmtree(extracted_dir, ignore_errors=True)


def validate_submission():
    print("=== {} {} Validator ===".format(course, conf["assignment_version"]))

    try:
        with extract_submission_temp(conf["submission_name"]) as (archive_filename, extracted_dir):
            print("Successfully extracted {}".format(archive_filename))

            validate_contents(archive_filename, extracted_dir, conf["specified_files"])
            print("File structure is correct.")

            print("\nVALIDATION SUCCEEDED.")
            print("\nDISCLAIMER: This does not mean that your submission is correct "
                  "- just that it appears to be structured correctly.")

    except ValidationException as e:
        print("\nVALIDATION FAILED")
        print(e)
        print("Please fix this and try validating again.")
    except Exception as e:
        print("\nVALIDATION FAILED")
        print("Exception occurred: {}".format(e))
        print("Stopping validation. Please fix this and try again.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    description='''This script helps you to verify that your submission structure is correct for {}.
Usage:
  1. Place this file in the same directory as your submission file, which is always
     a compressed archive. For this assignment, it must be named "{}.tar.gz"
     or "{}.zip".
  2. Run the program using:
        python3 submission_validator.py
  3. The verification results will be shown.
      - If you see "VALIDATION SUCCEEDED", your submission appears to be
        structured correctly.
      - Otherwise, if you see "VALIDATION FAILED", you should examine the
        error message, fix the problem, and try again.
                                '''.format(conf["assignment_version"], conf["submission_name"], conf["submission_name"], conf["submission_name"]),
                                formatter_class = argparse.RawTextHelpFormatter

                    )
    args = parser.parse_args()

    validate_submission()


