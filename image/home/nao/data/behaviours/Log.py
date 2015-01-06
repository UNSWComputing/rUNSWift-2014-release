import logging
import os

logger = logging.getLogger('behaviour')
logger.setLevel(logging.DEBUG) # Set this to logging.INFO for comp

log_folders = os.listdir('/var/volatile/runswift')
log_folders.sort()
log_folder = '/var/volatile/runswift/%s/behaviour' % log_folders[-1]

file_handler = logging.FileHandler(log_folder)
logger.addHandler(file_handler)

# Comment this handler out for competition
console_handler = logging.StreamHandler()
logger.addHandler(console_handler)

# Not sure how useful it would be to timestamp *every* message, might leave this to the user when required
# formatter = logging.Formatter(fmt = '%(created)f:%(message)s')
# handler.setFormatter(formatter)

def debug(*args, **kwargs):
    logger.debug(*args, **kwargs)

def info(*args, **kwargs):
    logger.info(*args, **kwargs)

def warning(*args, **kwargs):
    logger.warning(*args, **kwargs)

def error(*args, **kwargs):
    logger.error(*args, **kwargs)

def critical(*args, **kwargs):
    logger.critical(*args, **kwargs)
